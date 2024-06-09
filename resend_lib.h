#ifndef RESEND_LIB_H
#define RESEND_LIB_H

#include <memory>
#include "game_master.h"
#include "player_class.h"
#include <semaphore>

namespace resend_lib
{
    int resend_DEAL(int client_fd, 
                    std::shared_ptr<gm::GameMaster> game_master_sp, 
                    std::shared_ptr<Player> player_sp);

    int resend_TAKEN(int client_fd, 
                    std::shared_ptr<gm::GameMaster> game_master_sp, 
                    std::shared_ptr<Player> player_sp);

    int resend_TRICK(int client_fd, 
                    std::shared_ptr<gm::GameMaster> game_master_sp, 
                    std::shared_ptr<Player> player_sp,
                    std::shared_ptr<bool> resend_TRICK_msg);
                    
} // namespace resend_lib

#endif // RESEND_LIB_H