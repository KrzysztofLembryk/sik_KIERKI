#include "parameters_handling.h"

namespace po = boost::program_options;

#include <iostream>
#include <cinttypes>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <csignal>
#include <thread>
#include <atomic>
#include "constants.h"
#include "exception_wrappers.h"
#include "read_file.h"
#include "game_master.h"
#include "init_comm_wrappers.h"
#include "socket_fd_wrapper.h"
#include "ingame_comm_wrappers.h"
#include "thread_func.h"

void set_timeout_for_socket(int client_fd, int max_wait)
{
    struct timeval time_o = {.tv_sec = max_wait, .tv_usec = 0};
    if (setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &time_o, sizeof(time_o)) < 0)
    {
        exception_wrappers::runtime_err_wrapper("setsockopt() failed");
    }
}

void init_socket_fd(int &socket_fd)
{
    socket_fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        exception_wrappers::runtime_err_wrapper("socket() failed");
    }

    // Disabling IPV6_V6ONLY option so that we can use both IPv4 and IPv6 on
    // the same socket.
    int no = 0;
    if (setsockopt(socket_fd, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&no, sizeof(no)) == -1)
    {
        exception_wrappers::runtime_err_wrapper("setsockopt() failed");
    }
}

void handle_socket_init(uint16_t &port,
                        int &socket_fd,
                        struct sockaddr_in6 &server_address)
{
    // std::signal(SIGPIPE, SIG_IGN);
    init_socket_fd(socket_fd);

    server_address.sin6_family = AF_INET6;
    server_address.sin6_addr = in6addr_any;
    server_address.sin6_port = htons(port);

    // Now we need to bind created address to our socket.
    std::cout << "Binding to port " << port << "\n";

    if (bind(socket_fd, (struct sockaddr *)(&server_address),
             (socklen_t)sizeof (server_address)) < 0)
    {
        exception_wrappers::runtime_err_wrapper("binding socket with address unsuccesful");
    }

    // Switch the socket to listening.
    if (listen(socket_fd, QUEUE_LENGTH) < 0)
    {
        exception_wrappers::runtime_err_wrapper("listen() failed");
    }

    socklen_t length = (socklen_t) sizeof (server_address);
    if (getsockname(socket_fd, (struct sockaddr *) &server_address, &length) < 0)
    {
        exception_wrappers::runtime_err_wrapper("getsockname() failed");
    }
}

int init_server(int ac, char *av[], po::variables_map &vm,
                 uint16_t &port, unsigned &timeout, std::string &file_name,
                 int &socket_fd, struct sockaddr_in6 &server_address)
{
    try
    {
        parse_programme_parameters_server(ac, av, vm);
        // Default value for port is 0, since if port is not specified by user
        // 0 means we will bind to any available port.
        assign_programme_parameters_server(vm, port, timeout, file_name);
        print_parameters(port, timeout, file_name);
        // Read from file_name
        handle_socket_init(port, socket_fd, server_address);
        return SUCCESS;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return FAILURE;
    }
    
}

void print_client_address(struct sockaddr_in6 &client_address)
{
    char client_ip[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, &(client_address.sin6_addr), client_ip, INET6_ADDRSTRLEN);
    uint16_t client_port = ntohs(client_address.sin6_port);
    printf("accepted connection from %s:%" PRIu16 "\n", client_ip, client_port);
}

int main(int ac, char *av[])
{
    po::variables_map vm;
    uint16_t port = 0;
    unsigned timeout;
    std::string file_name;
    int socket_fd;
    struct sockaddr_in6 server_address;

    if (init_server(ac, av, vm, port, timeout, file_name, socket_fd, 
    server_address) != SUCCESS)
        return FAILURE;

    std::vector<gameCls::Round> vec_of_rounds;

    try
    {
     vec_of_rounds = fHandler::read_rounds_from_file(file_name);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return FAILURE;
    }
     
    std::shared_ptr<gm::GameMaster> game_master_sp = std::make_shared<gm::GameMaster>(vec_of_rounds, server_address);

    while (true)
    {
        try 
        {
            struct sockaddr_in6 client_address;
            socklen_t client_address_len = sizeof client_address;
            
            std::shared_ptr<ClientFdWrapper> client_fd_sp = std::make_shared<ClientFdWrapper>(
                accept(socket_fd, (struct sockaddr *) &client_address, 
                &client_address_len)
            );
            client_fd_sp->set_timeout_for_socket(timeout);

            print_client_address(client_address);

            init_comm_wrappers::IAM_Wrapper iam_wrapper;
            PlayerPosition new_p_position;
            if (iam_wrapper.read(client_fd_sp->to_int(), new_p_position) != SUCCESS)
            {
                continue;
            }

            if (game_master_sp->check_if_position_taken(new_p_position))
            {
                init_comm_wrappers::BUSY_Wrapper busy_wrapper;
                std::cout << "Sending BUSY packet\n";
                busy_wrapper.write(client_fd_sp->to_int(), game_master_sp->get_taken_positions());
                continue;
            }
            else 
            {
                game_master_sp->add_new_player(new_p_position, client_address);
            }

            std::thread t(thread_func::thread_main, client_fd_sp, game_master_sp, game_master_sp->get_player(new_p_position)
            );
            t.detach(); 
        }
        catch (std::exception &e)
        {
            std::cerr << e.what() << "\n";
        }
    }
    return SUCCESS;
}
