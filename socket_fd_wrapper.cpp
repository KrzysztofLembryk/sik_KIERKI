#include "socket_fd_wrapper.h"
#include "exception_wrappers.h"
#include <sys/socket.h>
#include <unistd.h>
#include "err.h"
#include <iostream>

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