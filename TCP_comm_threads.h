#ifndef TCP_COMM_THREADS_H
#define TCP_COMM_THREADS_H

#include <memory>
#include "game_master.h"
#include "socket_fd_handler.h"
#include <semaphore>

namespace TCP_threads
{
    class TCPThread
    {
    public:
        TCPThread() = default;
        ~TCPThread() = default;
        void TCP_thread_main(
            std::shared_ptr<ClientFdWrapper> client_fd_sp,
            std::shared_ptr<gm::GameMaster> game_master_sp,
            std::shared_ptr<Player> player_sp,
            std::shared_ptr<std::binary_semaphore> TCP_sem,
            int parent_pipe_read_fd);
    };
} // namespace TCP_threads

#endif // TCP_COMM_THREADS_H