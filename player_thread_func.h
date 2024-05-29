#ifndef PLAYER_THREAD_FUNC_H
#define PLAYER_THREAD_FUNC_H

#include <memory>
#include "socket_fd_handler.h"
#include "game_master.h"

namespace player_threads
{
    class MyThread
    {
    public:
        MyThread() = default;
        ~MyThread() = default;
        void thread_main(
            std::shared_ptr<ClientFdWrapper> client_fd_sp,
            std::shared_ptr<gm::GameMaster> game_master_sp,
            std::shared_ptr<Player> player_sp,
            int parent_pipe_write_fd);
    private:

    };
};

#endif // THREAD_FUNC_H