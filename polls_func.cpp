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

void polls_func::handle_polls_read(char *pipe_buff, int *pipe_fd)
{

    std::memset(pipe_buff, 0, PIPE_BUFF_SIZE);
    int bytes_read = read(pipe_fd[PIPE_READ_DSCR], pipe_buff, PIPE_BUFF_SIZE);
    if (bytes_read != PIPE_BUFF_SIZE)
    {
        exception_wrappers::runtime_err_wrapper("read() failed - wrong number of bytes read != 3");
    }
    if (strncmp(pipe_buff, "END", 3) == 0)
    {
        std::cout << "Received END signal\n";
    }
    else
    {
        exception_wrappers::runtime_err_wrapper("read() failed - wrong message received");
    }
}