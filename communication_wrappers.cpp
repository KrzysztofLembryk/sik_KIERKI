#include "communication_wrappers.h"
#include "common.h"
#include "err.h"
// #include <errno.h>
#include "exception_wrappers.h"
#include <cstring>
#include <iostream>

/**
 * When writen FAILS we will end thread/client_programme
*/


// - Function sends given packet of size packet_size to client_fd using writen
// - Function returns ERROR when writen returns <= 0 (also handles EPIPE) or 
// when writen size is not equal packet_size, otherwise it returns SUCCESS
void  TCP_send_packet(int socket_fd, void *packet, size_t packet_size)
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

int TCP_read_packet(int socket_fd, char *buff, size_t data_size,  
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

PlayerPosition check_read_pos(char pos)
{
    if (pos == 'N')
        return PlayerPosition::N;
    else if (pos == 'E')
        return PlayerPosition::E;
    else if (pos == 'S')
        return PlayerPosition::S;
    else if (pos == 'W')
        return PlayerPosition::W;
    else
    {
        err_func::error("Read wrong position");
        return PlayerPosition::NONE_POS;
    }

}

// IAM_Wrapper
void communication_wrappers::IAM_Wrapper::write(int socket_fd, PlayerPosition position)
{
    iam.position = static_cast<char>(position);
    TCP_send_packet(socket_fd, &iam, sizeof(iam));    
}

int communication_wrappers::IAM_Wrapper::read(int socket_fd)
{
    ssize_t read_length = 0;

    std::memset(read_buff, 0, IAM_BUFF_SIZE);
    
    if (TCP_read_packet(socket_fd, read_buff, IAM_BUFF_SIZE, read_length) != SUCCESS)
    {
        return ERROR;
    }

    if (strncmp(read_buff, "IAM", 3) != 0)
    {
        err_func::error("packet name not equal 'IAM'");
        return ERROR;
    }
    if (read_length != IAM_BUFF_SIZE)
    {
        err_func::error("read_length != IAM_BUFF_SIZE");
        return ERROR;
    }

    this->position = check_read_pos(read_buff[3]);

    if (this->position == PlayerPosition::NONE_POS)
    {
        return ERROR;
    }

    return SUCCESS;
}

PlayerPosition communication_wrappers::IAM_Wrapper::get_position()
{
    return this->position;
}