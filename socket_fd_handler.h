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

namespace socket_func
{
    
    void set_timeout_for_socket(int client_fd, int max_wait);

    void handle_server_socket_init(uint16_t &port,
                            int &socket_fd,
                            struct sockaddr_in6 &server_address);
    
    void handle_client_socket_init(int &socket_fd, int type_of_ip);
} // namespace socket_func



#endif