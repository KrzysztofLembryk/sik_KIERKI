#include "thread_func.h"
#include <iostream>

void signal_end_to_main_thread(int pipe_write_fd)
{
    char pipe_buffer[] {'E', 'N', 'D'}; 
    write(pipe_write_fd, pipe_buffer, sizeof(pipe_buffer));
}

void thread_nmspc::MyThread::thread_main(
        std::shared_ptr<ClientFdWrapper> client_fd_sp,
        std::shared_ptr<gm::GameMaster> game_master_sp,
        std::shared_ptr<Player> player_sp,
        int pipe_write_fd)
{
    game_master_sp->wait_for_turn(player_sp->get_position()); 
    
    
}