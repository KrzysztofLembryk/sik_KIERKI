#include "polls_func.h"
#include "exception_wrappers.h"
#include "constants.h"
#include <memory>
#include <iostream>
#include <cstring>

void polls_func::handle_polls_waiting(int &poll_status, struct pollfd *poll_descriptors)
{
    for (int i = 0; i < POLLS_NBR_OF_DSCR; i++)
    {
        poll_descriptors[i].revents = 0;
    }

    poll_status = poll(poll_descriptors, POLLS_NBR_OF_DSCR, -1);
    if (poll_status == -1)
    {
        if (errno == EINTR)
        {
            exception_wrappers::runtime_err_wrapper("interrupted system call");
        }
        else
        {
            exception_wrappers::runtime_err_wrapper("poll() failed");
        }
    }
}

void polls_func::handle_polls_read(int pipe_fd, std::string &msg)
{
    char pipe_buff[INGAME_PACKET_NAME_SIZE];
    std::memset(pipe_buff, 0, INGAME_PACKET_NAME_SIZE);

    int bytes_read = read(pipe_fd, pipe_buff, INGAME_PACKET_NAME_SIZE);
    if (bytes_read < 0 )
    {
        exception_wrappers::runtime_err_wrapper("read() failed - wrong message received");
    }

    msg = std::string(pipe_buff, bytes_read);
}