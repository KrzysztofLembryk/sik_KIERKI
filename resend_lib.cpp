#include "resend_lib.h"
#include "constants.h"
#include "ingame_comm_wrappers.h"
#include "init_comm_wrappers.h"
#include "common.h"

using GM_sp = std::shared_ptr<gm::GameMaster>;
using Player_sp = std::shared_ptr<Player>;
using BinSem_sp = std::shared_ptr<std::binary_semaphore>;


int resend_lib::resend_DEAL(int client_fd, 
                            GM_sp game_master_sp, 
                            Player_sp player_sp)
{
    std::string msg;
    std::string address_str = communication_addresses_to_str(player_sp->get_server_address(), player_sp->get_client_address(), false);
    init_comm_wrappers::DEAL_Wrapper deal;

    try
    {
        deal.write(client_fd, game_master_sp->get_game_type(), 
        game_master_sp->get_first_player(), 
        player_sp->get_hand(), msg);
        print_log_from_write_thread_safe(address_str, msg, game_master_sp);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';

        print_log_from_write_thread_safe(address_str, msg, game_master_sp);
        return ERROR;
    }
    return SUCCESS;
}

int resend_lib::resend_TAKEN(int client_fd, GM_sp game_master_sp, Player_sp player_sp)
{
    ingame_comm_wrappers::TAKEN_Wrapper taken;
    std::string address_str = communication_addresses_to_str(
                                            player_sp->get_server_address(), 
                                            player_sp->get_client_address(), 
                                            false);
    std::string msg_str;
    try 
    {
        auto lewas_played = game_master_sp->get_lewas_played();
        // we dont need to pushback curr lewa to lewas_vec in game_master since 
        // its done when we prepare new lewa
        for (auto lewa : lewas_played)
        {
            taken.write(client_fd, lewa, lewa.get_player_who_took_lewa(), msg_str);

            print_log_from_write_thread_safe(address_str, msg_str, game_master_sp);
        }

    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';

        print_log_from_write_thread_safe(address_str, msg_str, game_master_sp);

        return ERROR;
    }
    return SUCCESS;
}