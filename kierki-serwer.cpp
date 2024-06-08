#include "parameters_handling.h"

namespace po = boost::program_options;

#include <unistd.h>
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
#include "socket_fd_handler.h"
#include "ingame_comm_wrappers.h"
#include "player_threads.h"
#include "polls_func.h"
#include "common.h"
#include "err.h"

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
        // Read from file_name
        socket_func::handle_server_socket_init(port, socket_fd, server_address);
        return SUCCESS;
    }
    catch (const std::exception &e)
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

int handle_game_joining_request(int socket_fd, unsigned timeout, std::shared_ptr<gm::GameMaster> game_master_sp, int pipe_write_fd, struct sockaddr_in6 &server_address)
{
    struct sockaddr_in6 client_address;
    socklen_t client_address_len = sizeof client_address;

    std::shared_ptr<ClientFdWrapper> client_fd_sp =
        std::make_shared<ClientFdWrapper>(accept(socket_fd,
                                                 (struct sockaddr *)&client_address,
                                                 &client_address_len));

    client_fd_sp->set_timeout_for_socket(timeout);

    print_client_address(client_address);

    init_comm_wrappers::IAM_Wrapper iam_wrapper;
    PlayerPosition new_p_position;

    std::string address_str = communication_addresses_to_str((struct sockaddr *)&server_address, (struct sockaddr *)&client_address, true);
    std::string msg_str;
    if (iam_wrapper.read(client_fd_sp->to_int(), new_p_position, msg_str) != SUCCESS)
    {
        // We print log from write even though we did reading, since at the 
        // beginning of this project I decided that IAM will read whole msg 
        // inside read, without splitting reading to read_packet name and then 
        // reading the rest of the packet.
        print_log_from_write_thread_safe(address_str, msg_str, game_master_sp);
        err_func::error("IAM read unsuccessful");
        return CONTINUE;
    }

    print_log_from_write_thread_safe(address_str, msg_str, game_master_sp);

    if (game_master_sp->check_if_position_taken(new_p_position))
    {
        init_comm_wrappers::BUSY_Wrapper busy_wrapper;

        address_str = communication_addresses_to_str((struct sockaddr *)&server_address, (struct sockaddr *)&client_address, false);

        busy_wrapper.write(client_fd_sp->to_int(), 
                            game_master_sp->get_taken_positions(), 
                            msg_str);
        print_log_from_write_thread_safe(address_str, msg_str, game_master_sp);

        return CONTINUE;
    }
    else
    {
        game_master_sp->add_new_player(new_p_position, client_address);
    }

    player_threads::MyThread my_thread;
    std::thread t(
        [client_fd_sp, game_master_sp, new_p_position, pipe_write_fd, my_thread]() mutable {
            my_thread.thread_main(client_fd_sp,
                                    game_master_sp,
                                    game_master_sp->get_player(new_p_position),
                                    pipe_write_fd); 
        }
    );

    t.detach();

    return SUCCESS;
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
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return FAILURE;
    }

    int pipe_fd[2];
    char pipe_buff[PIPE_BUFF_SIZE];
    if (pipe(pipe_fd) == -1)
    {
        err_func::syserr("Failed to create pipe");
        return FAILURE;
    }

    std::shared_ptr<gm::GameMaster> game_master_sp = std::make_shared<gm::GameMaster>(vec_of_rounds, server_address);

    struct pollfd poll_descriptors[POLLS_NBR_OF_DSCR];
    int poll_status;

    // We wait for event on socket_fd and pipe_fd.
    poll_descriptors[TCP_SOCKET_POLLS_ID].fd = socket_fd;
    poll_descriptors[TCP_SOCKET_POLLS_ID].events = POLLIN;
    poll_descriptors[PIPE_POLLS_ID].fd = pipe_fd[PIPE_READ_DSCR];
    poll_descriptors[PIPE_POLLS_ID].events = POLLIN;

    while (true)
    {
        try
        {
            polls_func::handle_polls_waiting(poll_status, poll_descriptors);

            if (poll_status > 0)
            {
                // First we check if we got END signal from threads, and if not
                // then we can check if we got any new connections.
                if (poll_descriptors[PIPE_POLLS_ID].revents & POLLIN)
                {
                    std::string msg;
                    polls_func::handle_polls_read(pipe_fd[PIPE_READ_DSCR], msg, false);
                    break;
                }
                if (poll_descriptors[TCP_SOCKET_POLLS_ID].revents & POLLIN)
                {
                    try
                    {
                        if (handle_game_joining_request(socket_fd, timeout, game_master_sp, pipe_fd[PIPE_WRITE_DSCR], server_address) != SUCCESS)
                        {
                            continue;
                        }
                    }
                    catch(const std::exception& e)
                    {
                        std::cerr << e.what() << '\n';
                        continue;
                    }
                }
            }
        }
        catch (std::exception &e)
        {
            std::cerr << e.what() << "\n";
            close(socket_fd);
            close(pipe_fd[PIPE_READ_DSCR]);
            close(pipe_fd[PIPE_WRITE_DSCR]);
            return FAILURE;
        }
    }

    close(socket_fd);
    close(pipe_fd[PIPE_READ_DSCR]);
    close(pipe_fd[PIPE_WRITE_DSCR]);

    return SUCCESS;
}
