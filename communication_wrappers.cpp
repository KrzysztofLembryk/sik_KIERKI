#include "communication_wrappers.h"
#include "common.h"
#include "err.h"
// #include <errno.h>
#include "exception_wrappers.h"
#include <cstring>
#include <iostream>
#include "TCP_handler.h"

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
    tcp::TCP_send_packet(socket_fd, &iam, sizeof(iam));    
}

int communication_wrappers::IAM_Wrapper::read(int socket_fd)
{
    ssize_t read_length = 0;

    std::memset(read_buff, 0, IAM_BUFF_SIZE);
    
    if (tcp::TCP_read_packet(socket_fd, read_buff, IAM_BUFF_SIZE, read_length) != SUCCESS)
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