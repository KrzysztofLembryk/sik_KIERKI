#ifndef SOCKET_FD_WRAPPER_H
#define SOCKET_FD_WRAPPER_H

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
    // void handle_socket_init(uint16_t &port,
    //                         int &socket_fd,
    //                         struct sockaddr_in6 &server_address);
private:
    int client_fd;
    bool should_be_closed;
};


#endif