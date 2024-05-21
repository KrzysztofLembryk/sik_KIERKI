#include <boost/program_options.hpp>

namespace po = boost::program_options;

#include <iostream>
#include <cinttypes>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <csignal>
#include <thread>
#include <atomic>
#include "constants.h"
#include "exception_wrappers.h"

int parse_programme_parameters(int ac, char *av[], po::variables_map &vm)
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("p,p", po::value<std::vector<uint16_t>>(), "<port nbr> on which server listens")
        ("f,f", po::value<std::vector<std::string>>(), "<file name> to read from")
        ("t,t", po::value<std::vector<unsigned>>(), "<timeout> in seconds");

    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return FAILURE;
    }
    if (vm.count("f") == 0)
    {
        exception_wrappers::invalid_arg_wrapper("The 'f' option is required but missing.");
    }
    return SUCCES;
}

void assign_programme_parameters(po::variables_map &vm, uint16_t &port, unsigned &timeout, std::string &file_name)
{
    if (vm.count("p"))
    {
        auto ports = vm["p"].as<std::vector<uint16_t>>();
        port = ports[0];
    }
    if (vm.count("t"))
    {
        auto timeouts = vm["t"].as<std::vector<unsigned>>();
        timeout = timeouts[0];
    }
    else
        timeout = 5;

    auto file_names = vm["f"].as<std::vector<std::string>>();
    file_name = file_names[0];
}

void print_parameters(uint16_t port, unsigned timeout, std::string file_name)
{
    std::cout << "port: " << port << "\n";
    std::cout << "timeout: " << timeout << "\n";
    std::cout << "file name: " << file_name << "\n";
}

void init_socket_fd(int *socket_fd)
{
    *socket_fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (*socket_fd < 0)
    {
        exception_wrappers::runtime_err_wrapper("socket() failed");
    }

    // Disabling IPV6_V6ONLY option so that we can use both IPv4 and IPv6 on
    // the same socket.
    int no = 0;
    if (setsockopt(*socket_fd, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&no, sizeof(no)) == -1)
    {
        exception_wrappers::runtime_err_wrapper("setsockopt() failed");
    }
}

void set_timeout_for_socket(int client_fd, int max_wait)
{
    struct timeval time_o = {.tv_sec = max_wait, .tv_usec = 0};
    if (setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &time_o, sizeof(time_o)) < 0)
    {
        exception_wrappers::runtime_err_wrapper("setsockopt() failed");
    }
}

void handle_socket_init(uint16_t &port,
                        int *socket_fd,
                        struct sockaddr_in &server_address)
{
    std::signal(SIGPIPE, SIG_IGN);
    init_socket_fd(socket_fd);

    server_address.sin_family = AF_INET6;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);

    // Now we need to bind created address to our socket.
    if (bind(*socket_fd, (struct sockaddr *)(&server_address),
             (socklen_t)sizeof server_address) < 0)
    {
        exception_wrappers::runtime_err_wrapper("binding socket with address unsuccesful");
    }

    // Switch the socket to listening.
    if (listen(*socket_fd, QUEUE_LENGTH) < 0)
    {
        exception_wrappers::runtime_err_wrapper("listen() failed");
    }

    socklen_t length = (socklen_t) sizeof server_address;
    if (getsockname(*socket_fd, (struct sockaddr *) &server_address, &length) < 0)
    {
        exception_wrappers::runtime_err_wrapper("getsockname() failed");
    }
}

int main(int ac, char *av[])
{
    try
    {
        po::variables_map vm;
        if (parse_programme_parameters(ac, av, vm) != SUCCES)
            return FAILURE;

        // Default value for port is 0, since if port is not specified by user
        // 0 means we will bind to any available port.
        uint16_t port = 0;
        unsigned timeout;
        std::string file_name;

        assign_programme_parameters(vm, port, timeout, file_name);
        print_parameters(port, timeout, file_name);

        // Read from file_name

        int socket_fd;
        struct sockaddr_in server_address;

        handle_socket_init(port, &socket_fd, server_address);

        while (true)
        {
            struct sockaddr_in client_address;
            socklen_t client_address_len = sizeof client_address;

            int client_fd = accept(socket_fd, (struct sockaddr *) &client_address, &client_address_len);
            if (client_fd < 0)
            {
                exception_wrappers::runtime_err_wrapper("accept() failed");
            }

            std::thread t(
                [client_fd, client_address, timeout]()
                {
                    set_timeout_for_socket(client_fd, timeout);
                    // handle_connection(client_fd, file_name);
                }
            );
            t.detach();
        }

    }
    catch (std::invalid_argument &e)
    {
        std::cerr << "invalid arg error: " << e.what() << "\n";
        return FAILURE;
    }
    catch (std::runtime_error &e)
    {
        std::cerr << "runtime error: " << e.what() << "\n";
        return FAILURE;
    }
    catch (std::exception &e)
    {
        std::cerr << "exception error: " << e.what() << "\n";
        return FAILURE;
    }
    catch (...)
    {
        std::cerr << "error: unknown exception\n";
        return FAILURE;
    }
    return SUCCES;
}
