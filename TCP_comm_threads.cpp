#include "TCP_comm_threads.h"
#include "constants.h"
#include <poll.h>
#include "polls_func.h"

void TCP_threads::TCPThread::TCP_thread_main(
    std::shared_ptr<ClientFdWrapper> client_fd_sp,
    std::shared_ptr<gm::GameMaster> game_master_sp,
    std::shared_ptr<Player> player_sp,
    std::shared_ptr<std::binary_semaphore> TCP_sem,
    int parent_pipe_read_fd)
{

    struct pollfd poll_descriptors[POLLS_NBR_OF_DSCR];
    int poll_status;

    // We wait for event on socket_fd and pipe_fd.
    poll_descriptors[TCP_SOCKET_POLLS_ID].fd = client_fd_sp->to_int();
    poll_descriptors[TCP_SOCKET_POLLS_ID].events = POLLIN;
    poll_descriptors[PIPE_POLLS_ID].fd = parent_pipe_read_fd;
    poll_descriptors[PIPE_POLLS_ID].events = POLLIN;

    while (true)
    {
        polls_func::handle_polls_waiting(poll_status, poll_descriptors);

        if (poll_status > 0)
        {
            // First we check if we got END signal from threads, and if not
            // then we can check if we got any new connections.
            if (poll_descriptors[PIPE_POLLS_ID].revents & POLLIN)
            {
                std::string msg;
                polls_func::handle_polls_read(parent_pipe_read_fd, msg);
            }
            if (poll_descriptors[TCP_SOCKET_POLLS_ID].revents & POLLIN)
            {
                // if (handle_game_joining_request(socket_fd, timeout, game_master_sp, pipe_fd[PIPE_WRITE_DSCR]) != SUCCESS)
                // {
                //     continue;
                // }
            }
        }
    }
}