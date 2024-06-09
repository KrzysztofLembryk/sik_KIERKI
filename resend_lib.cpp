#include "resend_lib.h"
#include "constants.h"
#include "ingame_comm_wrappers.h"
#include "init_comm_wrappers.h"
#include "common.h"
#include "TCP_handler.h"

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
        if (lewas_played.size() == 0)
        {
            return SUCCESS;
        }
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

int resend_lib::resend_TRICK(int client_fd, GM_sp game_master_sp, Player_sp player_sp, std::shared_ptr<bool> resend_TRICK_msg)
{
    ingame_comm_wrappers::TRICK_Wrapper trick;
    cardCls::Lewa lewa_with_good_id(game_master_sp->get_curr_lewa_nbr());
    ingame_comm_wrappers::WRONG_Wrapper wrong;
    std::string address_str;
    std::string msg_str;
    std::string packet_name;

    while (true)
    {
        std::string address_str = communication_addresses_to_str(player_sp->get_server_address(), player_sp->get_client_address(), false);
        try
        {
            *resend_TRICK_msg = true;

            trick.write(client_fd, *(game_master_sp->get_curr_lewa()), msg_str);
            print_log_from_write_thread_safe(address_str, msg_str, game_master_sp);
            // trick.read() sets lewa_id and adds cards to Lewa

            address_str = communication_addresses_to_str(player_sp->get_server_address(), player_sp->get_client_address(), true);

            int ret_code_read_name = tcp::TCP_read_packet_name(client_fd, INGAME_PACKET_NAME_SIZE, packet_name);

            cardCls::Lewa client_ret_lewa;
            int ret_code = trick.read(client_fd, client_ret_lewa, game_master_sp->get_curr_lewa_nbr(), msg_str);

            print_log_from_read_thread_safe(address_str, packet_name, msg_str, game_master_sp);

            if (ret_code_read_name != SUCCESS || ret_code != SUCCESS)
                return ERROR;
            if (packet_name != "TRICK")
                return ERROR;
            if (ret_code == TIMEOUT)
                continue;

            if (client_ret_lewa.size() != 1)
            {
                address_str = communication_addresses_to_str(player_sp->get_server_address(), player_sp->get_client_address(), false);
                wrong.write(client_fd, lewa_with_good_id, msg_str);

                print_log_from_write_thread_safe(address_str, msg_str, game_master_sp);
                continue;
            }

            cardCls::CardClassWrapper client_card = client_ret_lewa.get_cards_in_lewa()[0];

            Suit bottom_card_suit =
                game_master_sp->get_curr_lewa()->size() == 0 ? client_card.get_suit() : game_master_sp->get_curr_lewa()->get_cards_in_lewa()[0].get_suit();

            // We check if sent card is in players deck and if its not played
            if (player_sp->check_card_correctness(client_ret_lewa.get_cards_in_lewa()[0], bottom_card_suit) != SUCCESS)
            {
                address_str = communication_addresses_to_str(player_sp->get_server_address(), player_sp->get_client_address(), false);
                wrong.write(client_fd, lewa_with_good_id, msg_str);

                print_log_from_write_thread_safe(address_str, msg_str, game_master_sp);
                continue;
            }

            *resend_TRICK_msg = false;
            // If card is correct we add it to lewa and set that this card is
            // played in player hand
            game_master_sp->add_card_to_lewa(client_card);
            player_sp->set_card_played(client_card);
            return SUCCESS;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            return ERROR;
        }
    }
}