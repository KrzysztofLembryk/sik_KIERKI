#include "TCP_threads.h"
#include "constants.h"
#include <poll.h>
#include "polls_func.h"
#include "ingame_comm_wrappers.h"
#include "TCP_handler.h"
#include "err.h"


int handle_player_msg_at_wrong_time(int client_fd, uint8_t curr_round)
{
    std::string packet_name;
    if (tcp::TCP_read_packet_name(client_fd, INGAME_PACKET_NAME_SIZE, packet_name) != SUCCESS)
    {
        // end connection
        err_func::error("ENDING CONNECTION - GOT ERROR WHILE READING PACKET NAME");
        return ERROR;
    }
    if (packet_name != "TRICK")
    {
        // end connection
        err_func::error("ENDING CONNECTION - GOT WRONG PACKET NAME");
        return ERROR;
    }

    ingame_comm_wrappers::TRICK_Wrapper client_trick;
    cardCls::Lewa lewa;

    try 
    {
        if (client_trick.read(client_fd, lewa, curr_round) != SUCCESS)
        {
            // end connection
            err_func::error("ENDING CONNECTION - GOT ERROR WHILE READING TRICK - sent trick packet was invalid");
            return ERROR;
        }
    }
    catch (const std::exception &e)
    {
        // end connection
        err_func::error("ENDING CONNECTION -- GOT EXCEPTION WHILE READING TRICK - sent trick packet was invalid - either value or suit");
        return ERROR;
    }
    // After we read trick packet, we can send WRONG msg
    ingame_comm_wrappers::WRONG_Wrapper wrong;
    cardCls::Lewa wrong_lewa(curr_round);
    wrong.write(client_fd, wrong_lewa);
    return SUCCESS;
}


void TCP_threads::TCPThread::TCP_thread_main(
    std::shared_ptr<ClientFdWrapper> client_fd_sp,
    std::shared_ptr<gm::GameMaster> game_master_sp,
    std::shared_ptr<Player> player_sp,
    std::shared_ptr<std::binary_semaphore> semaphore_TCP,
    int parent_pipe_read_fd,
    std::shared_ptr<bool> thread_ended_sp)
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
        // If client disconnects, we signalize parent thread that client
        // disconnected when he shouldnt and end this thread
        if (polls_func::handle_polls_waiting(poll_status, poll_descriptors) == DISCONNECTED)
        {
            (*thread_ended_sp) = true;
            semaphore_TCP->release();
            return;
        }

        if (poll_status > 0)
        {
            std::cout << "TCP thread got sth on polls\n";
            // First we check if we got END signal from threads, and if not
            // then we can check if we got any new connections.
            if (poll_descriptors[PIPE_POLLS_ID].revents & POLLIN)
            {
                std::string msg;
                polls_func::handle_polls_read(parent_pipe_read_fd, msg, true);
                std::cout << "Got msg: " << msg << "\n";
                if (msg == "DEAL")
                {
                    std::cout << "DEAL\n";
                    fflush(stdout);
                }
                else if (msg == "TRICK")
                {
                    std::cout << "TRICK\n";
                    fflush(stdout);
                }
                else if (msg == "TAKEN")
                {

                }
                else if (msg == "SCORE")
                {

                }
                else if (msg == "TOTAL")
                {

                }
                else
                {
                    exception_wrappers::runtime_err_wrapper("Got wrong msg from parent thread");
                }
                
            }
            if (poll_descriptors[TCP_SOCKET_POLLS_ID].revents & POLLIN)
            {
                if (handle_player_msg_at_wrong_time(client_fd_sp->to_int(), game_master_sp->get_curr_round_nbr()) != SUCCESS)
                {
                    (*thread_ended_sp) = true;
                    semaphore_TCP->release();
                    return;
                }
            }
        }
    }
}