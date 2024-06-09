#include "klient_non_auto_lib.h"
#include <poll.h>
#include "constants.h"
#include "exception_wrappers.h"
#include <iostream>
#include <unistd.h>
#include "polls_func.h"

void init_polls(int socket_fd, int child_pipe_fd[2], struct pollfd poll_descriptors[POLLS_NBR_OF_DSCR])
{
    // We wait for event on socket_fd and pipe_fd.
    poll_descriptors[TCP_SOCKET_POLLS_ID].fd = socket_fd;
    poll_descriptors[TCP_SOCKET_POLLS_ID].events = POLLIN;
    poll_descriptors[PIPE_POLLS_ID].fd = child_pipe_fd[PIPE_READ_DSCR];
    poll_descriptors[PIPE_POLLS_ID].events = POLLIN;
}

int handle_server_communication()
{

}

// This function will handle TCP connection with server and also getting msg 
// from thread that takes care of client interface
int klient_non_auto_func::klient_non_auto_main(AddressWrapper &server_address,
                                               AddressWrapper &client_address,
                                               int socket_fd,
                                               PlayerPosition chosen_position)
{
    int child_pipe_fd[2];

    if (pipe(child_pipe_fd) == -1)
    {
        exception_wrappers::runtime_err_wrapper("Failed to create pipe");
    }
    struct pollfd poll_descriptors[POLLS_NBR_OF_DSCR];
    int poll_status;
    init_polls(socket_fd, child_pipe_fd, poll_descriptors);

    while (true)
    {

        if (polls_func::handle_polls_waiting(poll_status, poll_descriptors) == DISCONNECTED)
        {
            return ERROR;
        }

        if (poll_status > 0)
        {
            // First we check if we got END signal from threads, and if not
            // then we can check if we got any new connections.
            if (poll_descriptors[PIPE_POLLS_ID].revents & POLLIN)
            {
                std::string msg;
                polls_func::handle_polls_read(child_pipe_fd[PIPE_READ_DSCR], msg, true);
                if (msg == "TRICK")
                {

                }
                else
                {
                    exception_wrappers::runtime_err_wrapper("Got wrong msg from client thread");
                }
            }
            if (poll_descriptors[TCP_SOCKET_POLLS_ID].revents & POLLIN)
            {
                if (handle_server_communication() != SUCCESS)
                {
                    // inform client about the end of connection
                    return ERROR;
                }
            }
        }
    }
}