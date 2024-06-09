#include "player_threads.h"
#include <iostream>
#include "ingame_comm_wrappers.h"
#include "constants.h"
#include <semaphore>
#include <thread>
#include "card_classes.h"
#include "btwn_thread_comm.h"
#include "TCP_threads.h"


using GM_sp = std::shared_ptr<gm::GameMaster>;
using Player_sp = std::shared_ptr<Player>;
using BinSem_sp = std::shared_ptr<std::binary_semaphore>;
using Bool_sp = std::shared_ptr<bool>;
using ClientFd_sp = std::shared_ptr<ClientFdWrapper>;


void signal_end_to_main_thread(int parent_pipe_write_fd)
{
    char pipe_buffer[]{'E', 'N', 'D'};
    if (write(parent_pipe_write_fd, pipe_buffer, sizeof(pipe_buffer)) < 0)
    {
        exception_wrappers::runtime_err_wrapper("Player thread Failed to write to pipe to signal end to main thread");
    }
}

void create_TCP_thread(
    GM_sp game_master_sp,
    Player_sp player_sp,
    int child_pipe_fd[2],
    BinSem_sp semaphore_TCP,
    Bool_sp thread_ended_sp,
    Bool_sp player_was_disconnected_sp,
    Bool_sp resend_TRICK_msg)
{
    TCP_threads::TCPThread tcp_thread;

    std::thread t(
        [game_master_sp, player_sp, semaphore_TCP, child_pipe_fd, thread_ended_sp, player_was_disconnected_sp, resend_TRICK_msg, tcp_thread]() mutable
        {
            tcp_thread.TCP_thread_main(game_master_sp,
                                       player_sp,
                                       semaphore_TCP,
                                       child_pipe_fd[PIPE_READ_DSCR],
                                       thread_ended_sp,
                                       player_was_disconnected_sp,
                                       resend_TRICK_msg);
        });
    t.detach();

}

void handle_disconnection(GM_sp game_master_sp, 
                            Player_sp player_sp, 
                            BinSem_sp semaphore_TCP,
                            Bool_sp thread_ended_sp,
                            Bool_sp player_was_disconnected_sp,
                            Bool_sp resend_TRICK_msg,
                            int child_pipe_fd[2],
                            std::string &comm_msg)
{
    while (*thread_ended_sp)
    {
        // If thread ended it might or might not read our msg, thus to be sure 
        // we need to discard it, by closing pipe and making new one, because 
        // it might happen that we sent DEAL, thread didnt read it, player 
        // reconnects we make a new thread, it sends DEAL and all TAKEN 
        // and then it reads from pipe DEAL and sends it again and player 
        // resets  his hand and thinks its new round
        close(child_pipe_fd[PIPE_READ_DSCR]);
        close(child_pipe_fd[PIPE_WRITE_DSCR]);
        if (pipe(child_pipe_fd) == -1)
        {
            exception_wrappers::runtime_err_wrapper("Failed to create pipe");
        }
        *thread_ended_sp = false;
        // We close client_fd socket, and wait for new one
        player_sp->reset_client_fd();
        game_master_sp->set_player_left(player_sp->get_position());
        game_master_sp->acquire_disconnected_sem(player_sp->get_position());

        *player_was_disconnected_sp = true;

        create_TCP_thread(game_master_sp, player_sp, child_pipe_fd, semaphore_TCP, thread_ended_sp, player_was_disconnected_sp, resend_TRICK_msg);
        
        // We wait for newly created thread to send all DEAL msgs to client
        // so we can continue
        semaphore_TCP->acquire();

        if (!(*thread_ended_sp))
        {
            btwn_thread_comm::send_msg(child_pipe_fd[PIPE_WRITE_DSCR], comm_msg);

            semaphore_TCP->acquire();
        }
    }
}

void handle_btwn_thread_comm(std::string comm_type, 
                            GM_sp game_master_sp, 
                            Player_sp player_sp, 
                            BinSem_sp semaphore_TCP, 
                            int child_pipe_fd[2], 
                            Bool_sp thread_ended_sp,
                            Bool_sp was_player_disconnected_sp,
                            Bool_sp resend_TRICK_msg)
{
    btwn_thread_comm::send_msg(child_pipe_fd[PIPE_WRITE_DSCR], comm_type);

    semaphore_TCP->acquire();

    handle_disconnection(game_master_sp, 
                        player_sp, 
                        semaphore_TCP,
                        thread_ended_sp, 
                        was_player_disconnected_sp,
                        resend_TRICK_msg,
                        child_pipe_fd,
                        comm_type);
}

void player_threads::MyThread::thread_main(
                                            GM_sp game_master_sp,
                                            Player_sp player_sp,
                                            int parent_pipe_write_fd)
{
    int child_pipe_fd[2];

    if (pipe(child_pipe_fd) == -1)
    {
        exception_wrappers::runtime_err_wrapper("Failed to create pipe");
    }

    BinSem_sp semaphore_TCP = std::make_shared<std::binary_semaphore>(0);
    Bool_sp thread_ended_sp = std::make_shared<bool>(false);
    Bool_sp player_was_disconnected_sp = std::make_shared<bool>(false);
    Bool_sp resend_TRICK_msg = std::make_shared<bool>(false);

    create_TCP_thread(game_master_sp, player_sp, child_pipe_fd, semaphore_TCP, thread_ended_sp, player_was_disconnected_sp, resend_TRICK_msg);

    size_t nbr_of_rounds = game_master_sp->get_nbr_of_rounds();
    for (size_t i = 0; i < nbr_of_rounds; i++)
    {
        // We need to check if game has started. If not we need to wait for all
        // other players, if it has and we are here it means that we are a new
        // player and we connected after sbs has disconnected
        game_master_sp->wait_for_game_start();

        // Here we send DEAL to All players
        handle_btwn_thread_comm("DEAL", game_master_sp, player_sp, semaphore_TCP, child_pipe_fd, thread_ended_sp, player_was_disconnected_sp, resend_TRICK_msg);

        sleep(5);
        game_master_sp->wait_for_all_players();

        while (true)
        {
            // After sending deal we wait for our turn to play card
            game_master_sp->wait_for_turn(player_sp->get_position());

            handle_btwn_thread_comm("TRICK", game_master_sp, player_sp, semaphore_TCP, child_pipe_fd, thread_ended_sp, player_was_disconnected_sp, resend_TRICK_msg);

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
            handle_btwn_thread_comm("TAKEN", game_master_sp, player_sp, semaphore_TCP, child_pipe_fd, thread_ended_sp, 
            player_was_disconnected_sp, resend_TRICK_msg);

            // We need to wait for other players so that all points for taken 
            // lewa is added for player who took it 
            game_master_sp->wait_for_all_players();

            if (game_master_sp->check_if_round_finished())
            {
                // Thanks to previous waiting now all players have scores up to 
                // date, and we can safely send scores to all players, while 
                // doing so we also add points from this round
                handle_btwn_thread_comm("SCORE", game_master_sp, player_sp, semaphore_TCP, child_pipe_fd, thread_ended_sp, player_was_disconnected_sp, resend_TRICK_msg);
                // Because we add points from round in SCORE we need to wait for
                // all players to finish adding points before we can send TOTAL
                // with updated scores
                game_master_sp->wait_for_all_players();

                handle_btwn_thread_comm("TOTAL", game_master_sp, player_sp, semaphore_TCP, child_pipe_fd, thread_ended_sp, player_was_disconnected_sp, resend_TRICK_msg);

                // IDK if we should wait for all players here, I think safer 
                // is to wait
                game_master_sp->wait_for_all_players();

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