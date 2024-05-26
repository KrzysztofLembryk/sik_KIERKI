#include "communication_wrappers.h"
#include "common.h"
#include "err.h"
// #include <errno.h>
#include "exception_wrappers.h"
#include <cstring>
#include <iostream>
#include "TCP_handler.h"

// Helper functions
std::vector<char> end_chars{'\r', '\n'};

PlayerPosition char_to_playerPos(char pos)
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
        err_func::error("Read not allowed position: " + std::string(1, pos));
        return PlayerPosition::NONE_POS;
    }
}

char playerPos_to_char(PlayerPosition pos)
{
    if (pos == PlayerPosition::N)
        return 'N';
    else if (pos == PlayerPosition::E)
        return 'E';
    else if (pos == PlayerPosition::S)
        return 'S';
    else if (pos == PlayerPosition::W)
        return 'W';
    else
    {
        exception_wrappers::invalid_arg_wrapper("Wrong position value");
    }
}

// IAM_Wrapper impl
/**
 * Function transforms PlayerPosition to correct char value and sends 
 * struct IAM by tcp socket
*/
void communication_wrappers::IAM_Wrapper::write(int socket_fd, PlayerPosition position)
{
    iam.position = playerPos_to_char(position);
    tcp::TCP_send_packet(socket_fd, &iam, sizeof(iam));    
}

/**
 * Function reads struct IAM from tcp socket, checks if nbr of bytes read is 
 * correct, checks if packet name is correct and then transforms char position 
 * value to PlayerPosition enum, and checks if it is correct
*/
int communication_wrappers::IAM_Wrapper::read(int socket_fd, PlayerPosition &position)
{
    ssize_t read_length = 0;

    std::memset(read_buff, 0, IAM_BUFF_SIZE);
    
    if (tcp::TCP_read_packet(socket_fd, read_buff, IAM_BUFF_SIZE, read_length) == TIMEOUT)
    {
        return TIMEOUT;
    }

    if (read_length != IAM_BUFF_SIZE)
    {
        err_func::error("read_length != IAM_BUFF_SIZE");
        return ERROR;
    }
    if (strncmp(read_buff, "IAM", 3) != 0)
    {
        err_func::error("packet name not equal 'IAM'");
        return ERROR;
    }

    position = char_to_playerPos(read_buff[3]);

    if (position == PlayerPosition::NONE_POS)
    {
        return ERROR;
    }

    return SUCCESS;
}

// BUSY_Wrapper

void communication_wrappers::BUSY_Wrapper::write(int socket_fd, std::vector<PlayerPosition> taken_positions)
{
    std::vector<char> msg_vec(name);

    // This should never HAPPEN if everything works correctly
    if (taken_positions.size() == 0)
    {
        msg_vec.insert(msg_vec.end(), end_chars.begin(), end_chars.end());
        tcp::TCP_send_packet(socket_fd, msg_vec.data(), msg_vec.size());
        return;
    }

    if (taken_positions.size() > 4)
        exception_wrappers::invalid_arg_wrapper("Too many taken positions");

    for (size_t i = 0; i < taken_positions.size(); i++)
        msg_vec.push_back(playerPos_to_char(taken_positions[i]));

    msg_vec.insert(msg_vec.end(), end_chars.begin(), end_chars.end());

    tcp::TCP_send_packet(socket_fd, msg_vec.data(), msg_vec.size());
}

/**
 *  We assume that first four bytes are read by function that called us, since
 * calling us means that these bytes were equal to BUSY
*/
int communication_wrappers::BUSY_Wrapper::read(
    int socket_fd, std::vector<PlayerPosition> &taken_positions)
{
    ssize_t read_length;
    char read_buff[BUSY_BUFF_SIZE - this->name.size()];
    std::memset(read_buff, 0, BUSY_BUFF_SIZE - this->name.size());

    // BUSY_BUFF_SIZE is maximally equal to 10 - 4 bytes for packet name = BUSY
    // 2 bytes for end chars and maximally 4 bytes for player positions thus
    // we can read maximally BUSY_BUFF_SIZE - this->name.size() bytes
    if (tcp::TCP_read_till_newline(socket_fd, read_buff, BUSY_BUFF_SIZE - this->name.size(), read_length) != SUCCESS)
    {
        return ERROR;
    }

    if (read_length == 2)
    {
        // Msg was BUSY\r\n thus no positions were taken
        return SUCCESS;
    }
    else if (read_length < 2)
    {
        err_func::error("read_length < 2");
        return ERROR;
    }
    else 
    {
        for (size_t i = 0; i < (size_t)(read_length - 2); i++)
        {
            PlayerPosition pos = char_to_playerPos(read_buff[i]);

            if (pos == PlayerPosition::NONE_POS)
            {
                err_func::error("Read wrong position");
                return ERROR;
            }

            taken_positions.push_back(char_to_playerPos(read_buff[i]));
        }
        return SUCCESS;
    }

}


