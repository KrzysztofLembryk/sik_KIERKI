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
            // exception_wrappers::runtime_err_wrapper("readn < 0");
            return ERROR;
        }
    }
    else if (read_length == 0) 
    {
        // exception_wrappers::runtime_err_wrapper(" - connection closed read_len == 0");
        err_func::error(" - connection closed read_len == 0");
        return DISCONNECTED;
    }
    return SUCCESS;
}

void add_rn_to_buff_if_needed(size_t &read_bytes, char *buff, size_t data_size)
{
    if (read_bytes == 0)
    {
        buff[0] = '\r';
        buff[1] = '\n';
        read_bytes = 2;
    }
    else if (read_bytes == 1)
    {
        if (buff[0] == '\r')
        {
            buff[1] = '\n';
            read_bytes = 2;
        }
        else
        {
            buff[1] = '\r';
            buff[2] = '\n';
            read_bytes = 3;
        }
    }
    else 
    {
        if (read_bytes < data_size - 2)
        {
            if (buff[read_bytes - 2] != '\r' || 
                (buff[read_bytes - 2] == '\r' && buff[read_bytes - 1] != '\n'))
            {
                buff[read_bytes] = '\r';
                read_bytes++;
                buff[read_bytes] = '\n';
                read_bytes++;
            }
        }
        else 
        {
            if (buff[read_bytes - 2] != '\r' || 
                (buff[read_bytes - 2] == '\r' && buff[read_bytes - 1] != '\n'))
            {
                buff[read_bytes - 2] = '\r';
                buff[read_bytes - 1] = '\n';
            }
        }
    }
}


int tcp::TCP_read_till_newline(int socket_fd, char *buff, size_t data_size, 
    ssize_t &total_bytes_read)
{
    total_bytes_read = 0;
    size_t read_bytes = 0;
    char curr_char = '\r';
    bool r_occured = false;
    int ret_code = SUCCESS;

    while(read_bytes < data_size)
    {
        ssize_t read_length = readn(socket_fd, &curr_char, 1);

        if (read_length < 0)
        {
            if (errno == EAGAIN) 
            {
                err_func::error("readn < 0 --> readn timeout");
                ret_code = TIMEOUT;
                break;
            } 
            else 
            {
                // exception_wrappers::runtime_err_wrapper("readn < 0");
                err_func::error("readn < 0");
                ret_code = ERROR;
                break;
            }
        }
        else if (read_length == 0) 
        {
            // exception_wrappers::runtime_err_wrapper("read_len == 0 -- no newline found in packet name or sent packet is to short or connection was closed");
            err_func::error("read_len == 0 -- client DISCONNECTED or packet was too short and didnt end with \\n");
            ret_code = DISCONNECTED;
            break;
        }

        // If curr_char is \r and then if next char is \n we end reading data
        // otherwise we set r_occured to false and continue reading 
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
        total_bytes_read++;

        if (curr_char == '\n' && r_occured)
            break;
    }

    add_rn_to_buff_if_needed(read_bytes, buff, data_size);
    std::cout.write(buff, read_bytes);

    if (ret_code != SUCCESS)
    {
        return ret_code;
    }
    // Sent packet must end with \n, thus this needs to be last character we 
    // read, otherwise packet is invalid
    if (curr_char != '\n')
    {
        err_func::error("send data didnt end with '\\n'");
        return ERROR;
    }

    return ret_code;
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
    
    int ret_code = SUCCESS;
    if (tcp::TCP_read_packet(socket_fd, name_buff, name_len, read_length) != SUCCESS)
    {
        ret_code = ERROR;
    }
    if (ret_code == SUCCESS && (size_t)read_length != name_len)
    {
        err_func::error("read_length != name_len");
        ret_code = ERROR;
    }

    if (read_length > 0)
        std::cout.write(name_buff, read_length);

    name = std::string(name_buff, name_len);
    return ret_code;
}