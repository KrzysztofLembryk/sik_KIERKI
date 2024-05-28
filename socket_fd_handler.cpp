#include "socket_fd_handler.h"
#include "exception_wrappers.h"
#include <sys/socket.h>
#include <unistd.h>
#include "err.h"
#include <iostream>
#include "constants.h"

ClientFdWrapper::ClientFdWrapper(int client_fd) : should_be_closed(false)
{
    if (client_fd < 0)
    {
        exception_wrappers::runtime_err_wrapper("client_socket_fd < 0 -- accept() failed");
    }
    this->should_be_closed = true;
    this->client_fd = client_fd;
}

ClientFdWrapper::~ClientFdWrapper()
{
    if (should_be_closed)
    {
        close(client_fd);
    }
}

void ClientFdWrapper::set_timeout_for_socket(unsigned max_wait)
{
    struct timeval time_o = {.tv_sec = max_wait, .tv_usec = 0};
    if (setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &time_o, sizeof(time_o)) < 0)
    {
        exception_wrappers::runtime_err_wrapper("setsockopt() failed");
    }
}

int ClientFdWrapper::to_int() const
{
    return client_fd;
}

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
             (socklen_t)sizeof(server_address)) < 0)
    {
        exception_wrappers::runtime_err_wrapper("binding socket with address unsuccesful");
    }

    // Switch the socket to listening.
    if (listen(socket_fd, QUEUE_LENGTH) < 0)
    {
        exception_wrappers::runtime_err_wrapper("listen() failed");
    }

    socklen_t length = (socklen_t)sizeof(server_address);
    if (getsockname(socket_fd, (struct sockaddr *)&server_address, &length) < 0)
    {
        exception_wrappers::runtime_err_wrapper("getsockname() failed");
    }
}
