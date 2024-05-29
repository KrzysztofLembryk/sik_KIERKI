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
    char pipe_buffer[] {'E', 'N', 'D'}; 
    write(parent_pipe_write_fd, pipe_buffer, sizeof(pipe_buffer));
}

void player_threads::MyThread::thread_main(
        std::shared_ptr<ClientFdWrapper> client_fd_sp,
        std::shared_ptr<gm::GameMaster> game_master_sp,
        std::shared_ptr<Player> player_sp,
        int parent_pipe_write_fd)
{
    std::cout << "Thread started\n";
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
        }
    );

    t.detach();

    while(true)
    {
        // If game has started and we are here, this means that we are a new 
        // player and are just connecting to the game. Thus we shouldnt wait for
        // game to start
        // if (!game_master_sp->check_if_game_started())
        //     game_master_sp->wait_for_game_start();

        // Here we send DEAL to All players 
        btwn_thread_comm::send_msg(child_pipe_fd[PIPE_WRITE_DSCR], "DEAL");
        
        game_master_sp->wait_for_turn(player_sp->get_position()); 

        // Player whose turn is needs to get TRICK from server
        btwn_thread_comm::send_msg(child_pipe_fd[PIPE_WRITE_DSCR], "TRICK");

        // After we told helping thread to send TRICK, we wait for player 
        // response, once we get it helping thread checks if it is valid and if
        // it is it releases the semaphore
        semaphore_TCP->acquire();

        std::shared_ptr<cardCls::Lewa> curr_lewa = 
                                                game_master_sp->get_curr_lewa();
        

    }
    
    close(child_pipe_fd[PIPE_READ_DSCR]);
    close(child_pipe_fd[PIPE_WRITE_DSCR]); 
}