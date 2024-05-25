#include "communicates_wrappers.h"
#include "common.h"
#include <errno.h>
#include "constants.h"
#include "exception_wrappers.h"
#include <cstring>

/**
 * When writen FAILS we will end thread/client_programme
*/


// - Function sends given packet of size packet_size to client_fd using writen
// - Function returns ERROR when writen returns <= 0 (also handles EPIPE) or 
// when writen size is not equal packet_size, otherwise it returns SUCCESS
void TCP_send_packet(int socket_fd, void *packet, size_t packet_size)
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

void TCP_read_packet(int socket_fd, char *buff, size_t data_size)
{
    ssize_t read_length = readn(socket_fd, buff, data_size);

    if (read_length < 0)
    {
        if (errno == EAGAIN) 
        {
            exception_wrappers::runtime_err_wrapper("readn < 0 --> readn timeout");
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
}

// IAM_Wrapper
void IAM_Wrapper::write(int socket_fd, PlayerPosition position)
{
    iam.position = static_cast<char>(position);
    TCP_send_packet(socket_fd, &iam, sizeof(iam));    
}

void IAM_Wrapper::read(int socket_fd, PlayerPosition &position)
{
    std::memset(read_buff, 0, IAM_BUFF_SIZE);
    ssize_t read_length = readn(socket_fd, read_buff, sizeof(iam));

    if (strncmp(read_buff, "IAM", 3) != 0)
    {
        exception_wrappers::runtime_err_wrapper("strncmp(buff, 'IAM', 3) != 0 --> wrong packet name");
    }

    position = static_cast<PlayerPosition>(read_buff[3]);
}