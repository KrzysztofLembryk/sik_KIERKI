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
            return TIMEOUT;
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

int tcp::TCP_read_till_newline(int socket_fd, char *buff, size_t data_size, 
    ssize_t &read_length)
{
    size_t read_bytes = 0;
    char curr_char = '\r';
    bool r_occured = false;

    while(read_bytes < data_size)
    {
        read_length = readn(socket_fd, &curr_char, 1);

        if (read_length < 0)
        {
            if (errno == EAGAIN) 
            {
                err_func::error("readn < 0 --> readn timeout");
                return TIMEOUT;
            } 
            else 
            {
                exception_wrappers::runtime_err_wrapper("readn < 0");
            }
        }
        else if (read_length == 0) 
        {
            exception_wrappers::runtime_err_wrapper("read_len == 0 -- no newline found in packet name or sent packet is to short or connection was closed");
        }
        if (curr_char == '\r')
            r_occured = true;
        else 
        {
            if (r_occured && curr_char != '\n')
            {
                r_occured = false;
            }
        }

        buff[read_bytes] = curr_char;
        read_bytes++;

        if (curr_char == '\n' && r_occured)
            break;
    }

    if (curr_char != '\n')
    {
        err_func::error("curr_char != '\\n'");
        return ERROR;
    }
    read_length = read_bytes;

    return SUCCESS;
}

int tcp::TCP_read_packet_name(int socket_fd, size_t name_len, std::string &name)
{
    if (name_len > MAX_PACKET_NAME_SIZE)
    {
        err_func::error("name_len > MAX_PACKET_NAME_SIZE");
        return ERROR;
    }
    char name_buff[MAX_PACKET_NAME_SIZE];
    ssize_t read_length;

    std::memset(name_buff, 0, MAX_PACKET_NAME_SIZE);
    if (tcp::TCP_read_packet(socket_fd, name_buff, name_len, read_length) != SUCCESS)
    {
        return ERROR;
    }
    if ((size_t)read_length != name_len)
    {
        err_func::error("read_length != name_len");
        return ERROR;
    }

    name = std::string(name_buff, name_len);
    return SUCCESS;
}