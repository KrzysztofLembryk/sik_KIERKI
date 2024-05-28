#ifndef SOCKET_FD_WRAPPER_H
#define SOCKET_FD_WRAPPER_H

#include <netinet/in.h>

class ClientFdWrapper
{
public:
    ClientFdWrapper() = delete;
    ClientFdWrapper(int client_fd);
    ClientFdWrapper(const ClientFdWrapper& other) = default;
    ClientFdWrapper& operator=(const ClientFdWrapper& other) = default;
    ~ClientFdWrapper();

    void set_timeout_for_socket(unsigned max_wait);
    int to_int() const;

private:
    int client_fd;
    bool should_be_closed;
};

void set_timeout_for_socket(int client_fd, int max_wait);

void init_socket_fd(int &socket_fd);

void handle_socket_init(uint16_t &port,
                        int &socket_fd,
                        struct sockaddr_in6 &server_address);

#endif