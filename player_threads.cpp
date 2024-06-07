#include "player_threads.h"
#include <iostream>
#include "ingame_comm_wrappers.h"
#include "constants.h"
#include <semaphore>
#include <thread>
#include "card_classes.h"
#include "btwn_thread_comm.h"
#include "TCP_threads.h"

void signal_end_to_main_thread(int parent_pipe_write_fd)
{
    char pipe_buffer[]{'E', 'N', 'D'};
    write(parent_pipe_write_fd, pipe_buffer, sizeof(pipe_buffer));
}

void player_threads::MyThread::thread_main(
    std::shared_ptr<ClientFdWrapper> client_fd_sp,
    std::shared_ptr<gm::GameMaster> game_master_sp,
    std::shared_ptr<Player> player_sp,
    int parent_pipe_write_fd)
{
    int child_pipe_fd[2];
    char pipe_buff[INGAME_PACKET_NAME_SIZE];

    if (pipe(child_pipe_fd) == -1)
    {
        exception_wrappers::runtime_err_wrapper("Failed to create pipe");
    }

    std::shared_ptr<std::binary_semaphore> semaphore_TCP = std::make_shared<std::binary_semaphore>(0);
    std::shared_ptr<bool> thread_ended_sp = std::make_shared<bool>(false);

    TCP_threads::TCPThread tcp_thread;

    std::thread t(
        [client_fd_sp, game_master_sp, player_sp, semaphore_TCP, child_pipe_fd, thread_ended_sp, tcp_thread]() mutable
        {
            tcp_thread.TCP_thread_main(client_fd_sp,
                                       game_master_sp,
                                       player_sp,
                                       semaphore_TCP,
                                       child_pipe_fd[PIPE_READ_DSCR],
                                       thread_ended_sp);
        });
    t.detach();

    uint8_t nbr_of_rounds = game_master_sp->get_nbr_of_rounds();
    for (uint8_t i = 0; i < nbr_of_rounds; i++)
    {
        // We need to check if game has started. If not we need to wait for all
        // other players, if it has and we are here it means that we are a new
        // player and we connected after sbs has disconnected
        game_master_sp->wait_for_game_start();

        // Here we send DEAL to All players
        btwn_thread_comm::send_msg(child_pipe_fd[PIPE_WRITE_DSCR], "DEAL");
        semaphore_TCP->acquire();

        while (true)
        {
            // After sending deal we wait for our turn to play card
            game_master_sp->wait_for_turn(player_sp->get_position());

            // Player whose turn is needs to get TRICK from server
            btwn_thread_comm::send_msg(child_pipe_fd[PIPE_WRITE_DSCR], "TRICK");

            // After we told helping thread to send TRICK, we wait for player
            // response, once we get it helping thread checks if it is valid 
            // and if it is, it releases the semaphore, otherwise it sends 
            // WRONG and waits for player's response again
            semaphore_TCP->acquire();

            // Here if we are last player who plays card, we need to check who 
            // won lewa, count cards that has been played and then release 
            // barrier
            if (game_master_sp->check_if_curr_lewa_full())
            {
                game_master_sp->check_who_won_lewa();
                game_master_sp->set_first_player_for_next_turn();
                game_master_sp->count_cards_played();
                game_master_sp->wait_for_all_players();
            }
            else
            {
                game_master_sp->next_player_turn();
                game_master_sp->wait_for_all_players();
            }

            // We know who won lewa thus we can send TAKEN to all players
            btwn_thread_comm::send_msg(child_pipe_fd[PIPE_WRITE_DSCR], "TAKEN");
            semaphore_TCP->acquire();

            // We need to wait for other players so that all points for taken 
            // lewa is added for player who took it 
            game_master_sp->wait_for_all_players();

            if (game_master_sp->check_if_round_finished())
            {
                btwn_thread_comm::send_msg(child_pipe_fd[PIPE_WRITE_DSCR], "SCORE");
                semaphore_TCP->acquire();

                game_master_sp->wait_for_all_players();

                btwn_thread_comm::send_msg(child_pipe_fd[PIPE_WRITE_DSCR], "TOTAL");
                semaphore_TCP->acquire();
                
                if (game_master_sp->check_if_last_round())
                {
                    // Game has ended
                    if (game_master_sp->decrement_present_players() == 0)
                    {
                        signal_end_to_main_thread(parent_pipe_write_fd);
                    }
                    btwn_thread_comm::send_msg(child_pipe_fd[PIPE_WRITE_DSCR], "END");
                    semaphore_TCP->acquire();
                    close(child_pipe_fd[PIPE_READ_DSCR]);
                    close(child_pipe_fd[PIPE_WRITE_DSCR]);
                    return;
                }
                else
                {
                    // If round finished we wait for other players, so that 
                    // they can send lewa by TCP, if we firstly called 
                    // prepare_new_round we could clear lewa before everyone 
                    // would send it so we would have race conditions and hard 
                    // to detect bug
                    game_master_sp->wait_for_all_players();
                    game_master_sp->prepare_new_round();
                    break;
                }
            }
            else
            {
                game_master_sp->wait_for_all_players();
                game_master_sp->prepare_new_lewa();
            }
        }
    }
}