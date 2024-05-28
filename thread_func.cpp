#include "thread_func.h"
#include <iostream>

void thread_nmspc::MyThread::thread_main(
        std::shared_ptr<ClientFdWrapper> client_fd_sp,
        std::shared_ptr<gm::GameMaster> game_master_sp,
        std::shared_ptr<Player> player_sp,
        int pipe_write_fd)
{
    sleep(5);
    std::cout << "thread sending END\n";
    fflush(stdout);

    char pipe_buffer[] {'E', 'N', 'D'}; 
    write(pipe_write_fd, pipe_buffer, sizeof(pipe_buffer));
}