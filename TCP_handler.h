#ifndef TCP_HANDLER_H
#define TCP_HANDLER_H

#include <unistd.h>
#include <cstring>
// - Function sends given packet of size packet_size to client_fd using writen
// - Function returns ERROR when writen returns <= 0 (also handles EPIPE) or 
// when writen size is not equal packet_size, otherwise it returns SUCCESS
namespace tcp
{
    void TCP_send_packet(int socket_fd, void *packet, size_t packet_size);

    int TCP_read_packet(int socket_fd, char *buff, size_t data_size,  
        ssize_t &read_length);
}

#endif // TCP_HANDLER_H