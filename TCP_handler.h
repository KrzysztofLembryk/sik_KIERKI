#ifndef TCP_HANDLER_H
#define TCP_HANDLER_H

#include <unistd.h>
#include <cstring>
#include <string>
// - Function sends given packet of size packet_size to client_fd using writen
// - Function returns ERROR when writen returns <= 0 (also handles EPIPE) or 
// when writen size is not equal packet_size, otherwise it returns SUCCESS
namespace tcp
{
    void TCP_send_packet(int socket_fd, void *packet, size_t packet_size);

    int TCP_read_packet(int socket_fd, char *buff, size_t data_size,  
        ssize_t &read_length);
    
    int TCP_read_till_newline(int socket_fd, char *buff, size_t data_size, 
        ssize_t &total_bytes_read);
    
    int TCP_read_packet_name(int socket_fd, size_t name_len, std::string &name);
}

#endif // TCP_HANDLER_H