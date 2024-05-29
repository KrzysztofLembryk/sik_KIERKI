#include "player_thread_func.h"
#include <iostream>
#include "ingame_comm_wrappers.h"
#include "constants.h"
#include <semaphore>
#include "card_classes.h"

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
    int child_pipe_fd[2];
    char pipe_buff[INGAME_PACKET_NAME_SIZE];

    if (pipe(child_pipe_fd) == -1)
    {
        exception_wrappers::runtime_err_wrapper("Failed to create pipe");
    }

    std::shared_ptr<std::binary_semaphore> TCP_comm_sem = std::make_shared<std::binary_semaphore>(0);

    while(true)
    {
        // If game has started and we are here, this means that we are a new 
        // player and are just connecting to the game. Thus we shouldnt wait for
        // game to start
        if (!game_master_sp->check_if_game_started())
            game_master_sp->wait_for_game_start();
        else 
        {
            // We should send all lewas etc to the player
        }
        game_master_sp->wait_for_turn(player_sp->get_position()); 

        std::shared_ptr<cardCls::Lewa> curr_lewa = 
                                                game_master_sp->get_curr_lewa();

    }
    
    
}