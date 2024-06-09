#ifndef CLIENT_INTERFACE_LIB_H
#define CLIENT_INTERFACE_LIB_H

#include <semaphore>
#include <memory>
#include "player_class.h"

namespace client_interface_lib
{
    class InterfaceThread
    {
        InterfaceThread() = default;
        ~InterfaceThread() = default;
        void interface_thread_main(
            std::shared_ptr<Player> player_sp,
            std::shared_ptr<std::binary_semaphore> TCP_sem,
            int parent_pipe_read_fd,
            int parent_pipe_write_fd,
            std::shared_ptr<bool> thread_ended_sp);
    };
}

#endif // CLIENT_INTERFACE_LIB_H