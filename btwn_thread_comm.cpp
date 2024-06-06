#include "btwn_thread_comm.h"
#include <unistd.h>
#include "constants.h"
#include "exception_wrappers.h"

void btwn_thread_comm::send_msg(int fd, std::string msg)
{
    int n = write(fd, msg.data(), msg.size());
    if (n < 0)
    {
        exception_wrappers::runtime_err_wrapper("Error writing to socket");
    }
}

void btwn_thread_comm::read_msg(int fd, std::string &msg)
{
    char buf[INGAME_PACKET_NAME_SIZE];
    int n = read(fd, buf, INGAME_PACKET_NAME_SIZE);
    if (n < 0)
    {
        exception_wrappers::runtime_err_wrapper("Error reading from socket");
    }
    msg = std::string(buf, n);
}