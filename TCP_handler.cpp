#include "TCP_handler.h"
#include "err.h"
#include "exception_wrappers.h"
#include "common.h"
#include "constants.h"

// - Function sends given packet of size packet_size to client_fd using writen
// - Function returns ERROR when writen returns <= 0 (also handles EPIPE) or 
// when writen size is not equal packet_size, otherwise it returns SUCCESS
void tcp::TCP_send_packet(int socket_fd, void *packet, size_t packet_size)
{
    ssize_t written_length = writen(socket_fd, packet, packet_size);

    if (written_length < 0 )
    {
        if (errno == EPIPE)
            exception_wrappers::runtime_err_wrapper("writen < 0 --> SIGPIPE signal in write, client closed reading end of socket before server could send msg");
        else
            exception_wrappers::runtime_err_wrapper("writen < 0 --> error in write");
    }
    if ((size_t) written_length < packet_size) 
    {
        exception_wrappers::runtime_err_wrapper("writen < packet_size --> writen wrote less than wanted size");
    }
    if (written_length == 0)
    {
        exception_wrappers::runtime_err_wrapper(" - writen len == 0");
    }
}

int tcp::TCP_read_packet(int socket_fd, char *buff, size_t data_size,  
    ssize_t &read_length)
{
    read_length = readn(socket_fd, buff, data_size);

    if (read_length < 0)
    {
        if (errno == EAGAIN) 
        {
            err_func::error("readn < 0 --> readn timeout");
            return ERROR;
        } 
        else 
        {
            exception_wrappers::runtime_err_wrapper("readn < 0");
        }
    }
    else if (read_length == 0) 
    {
        exception_wrappers::runtime_err_wrapper(" - connection closed read_len == 0");
    }
    return SUCCESS;
}


