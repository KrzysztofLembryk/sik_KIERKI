#include "klient_auto_lib.h"
#include "init_comm_wrappers.h"
#include "ingame_comm_wrappers.h"
#include "TCP_handler.h"
#include <string>
#include "common.h"
#include <memory>
#include "player_class.h"
#include "game_master.h"

void just_read_rest_of_wrong_packet(int socket_fd, size_t data_size, std::string &msg_str)
{
    char *buff = new char[data_size];
    ssize_t total_bytes_read = 0;

    tcp::TCP_read_till_newline(socket_fd, buff, data_size, total_bytes_read, msg_str);

    delete[] buff;
}

int handle_read_packet_name(int socket_fd, std::string &packet_name, 
struct sockaddr *server_address, struct sockaddr *client_address, 
size_t packet_name_size, size_t data_size)
{
    ssize_t read_length;
    std::string msg_str;
    std::string address_str = communication_addresses_to_str(server_address, client_address, false, NOT_INVOKED_BY_SERVER);

    int ret_val_read_name = tcp::TCP_read_packet_name(socket_fd, packet_name_size, packet_name);

    if (ret_val_read_name == DISCONNECTED)
    {
        return DISCONNECTED;
    }
    if (ret_val_read_name == FAILURE)
    {
        // Server sent wrong packet - we ignore it
        just_read_rest_of_wrong_packet(socket_fd, data_size, msg_str);
        print_log_from_read(address_str, packet_name, msg_str);
        return CONTINUE;
    }
    if (ret_val_read_name != SUCCESS)
    {
        return ERROR;
    }
    return SUCCESS;
}

int play_game(int socket_fd, std::shared_ptr<Player> player_sp)
{
    struct sockaddr *server_address = player_sp->get_server_address();
    struct sockaddr *client_address = player_sp->get_client_address();
    uint8_t curr_lewa_id = 1;
    bool got_score = false;
    bool got_total = false;

    while (true)
    {
        std::string packet_name;
        std::string addreses_str = communication_addresses_to_str(server_address, client_address, false, NOT_INVOKED_BY_SERVER);
        std::string msg_str;
        int ret_val_read_name;

        if (got_score && got_total)
        {
            ret_val_read_name = handle_read_packet_name(
                                                socket_fd, 
                                                packet_name, 
                                                player_sp->get_server_address(), 
                                                player_sp->get_client_address(), 
                                                INIT_PACKET_NAME_SIZE, 
                                                MAX_DEAL_BUFF_SIZE);
        }
        else 
        {
            ret_val_read_name = handle_read_packet_name(
                                                socket_fd, 
                                                packet_name, 
                                                player_sp->get_server_address(), 
                                                player_sp->get_client_address(), 
                                                INGAME_PACKET_NAME_SIZE, 
                                                MAX_TOTAL_BUFF_SIZE);

        }
        if (ret_val_read_name == CONTINUE)
            continue;
        else if (ret_val_read_name != SUCCESS)
        {
            // Game has ended
            if (got_score && got_total)
            {
                return SUCCESS;
            }
            return ret_val_read_name;
        }

        if (packet_name == "DEAL") 
        {
            init_comm_wrappers::DEAL_Wrapper deal;
            GameType game_type;
            PlayerPosition first_player_pos;
            cardCls::DeckOfCards my_hand;

            try 
            {
                int ret_val_deal = deal.read(socket_fd, game_type, first_player_pos, my_hand, msg_str); 
                print_log_from_read(addreses_str, packet_name, msg_str);

                if (ret_val_deal == FAILURE)
                {
                    continue;
                }
                else if (ret_val_deal != SUCCESS)
                {
                    return FAILURE;
                }

                curr_lewa_id = 1;
                got_score = false;
                got_total = false;
                player_sp->set_game_type(game_type);
                player_sp->set_hand(my_hand);
                player_sp->clea_lewas_taken();
            }
            catch (std::exception &e)
            {
                std::cerr << e.what() << "\n";
                print_log_from_read(addreses_str, packet_name, msg_str);
                continue;
            }
        }
        else if (packet_name == "TRICK")
        {
            ingame_comm_wrappers::TRICK_Wrapper trick;
            cardCls::Lewa lewa;
            try 
            {
                int ret_val_trick = trick.read(socket_fd, lewa, curr_lewa_id, msg_str);
                print_log_from_read(addreses_str, packet_name, msg_str);
                if (ret_val_trick == FAILURE)
                {
                    continue;
                }
                else if (ret_val_trick != SUCCESS)
                {
                    return FAILURE;
                }
                Suit bottom_card_suit;
                if (lewa.size() > 0)
                {
                    bottom_card_suit = lewa.get_cards_in_lewa()[0].get_suit();
                }
                else
                {
                    bottom_card_suit = Suit::NONE_SUIT;
                }
                auto playerd_card = 
                                player_sp->play_card(bottom_card_suit);
                cardCls::Lewa ret_lewa(lewa.get_lewa_id());

                ret_lewa.add_card(playerd_card);
                addreses_str = communication_addresses_to_str(server_address, client_address, true, NOT_INVOKED_BY_SERVER);
                trick.write(socket_fd, ret_lewa, msg_str);

                print_log_from_write(addreses_str, msg_str);
            }
            catch (std::exception &e)
            {
                std::cerr << e.what() << "\n";
                continue;
            }
        }
        else if (packet_name == "TAKEN")
        {
            ingame_comm_wrappers::TAKEN_Wrapper taken;
            cardCls::Lewa lewa;
            PlayerPosition player_who_took_lewa;
            try
            {
                int ret_val_taken = taken.read(socket_fd, lewa, player_who_took_lewa, curr_lewa_id, msg_str);
                print_log_from_read(addreses_str, packet_name, msg_str);

                if (ret_val_taken == FAILURE)
                {
                    continue;
                }
                else if (ret_val_taken != SUCCESS)
                {
                    return FAILURE;
                }

                if (player_sp->get_position() == player_who_took_lewa)
                {
                    player_sp->add_points_in_curr_round(lewa);
                    player_sp->add_lewa_to_lewas_taken(lewa);
                }
                curr_lewa_id++;
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
                continue;
            }
        }
        else if (packet_name == "SCORE")
        {
            ingame_comm_wrappers::SCORE_Wrapper score;
            std::map<PlayerPosition, uint8_t> scores;
            try 
            {
                int ret_val_score = score.read(socket_fd, scores, msg_str);
                print_log_from_read(addreses_str, packet_name, msg_str);

                if (ret_val_score == FAILURE)
                {
                    continue;
                }
                else if (ret_val_score != SUCCESS)
                {
                    return FAILURE;
                }
                got_score = true;
            }
            catch (std::exception &e)
            {
                std::cerr << e.what() << "\n";
                continue;
            }
        }
        else if (packet_name == "TOTAL")
        {
            ingame_comm_wrappers::TOTAL_Wrapper total;
            std::map<PlayerPosition, uint32_t> total_scores;
            try
            {
                int ret_val_total = total.read(socket_fd, total_scores, msg_str);
                print_log_from_read(addreses_str, packet_name, msg_str);
                if (ret_val_total == FAILURE)
                {
                    continue;
                }
                else if (ret_val_total != SUCCESS)
                {
                    return FAILURE;
                }
                player_sp->add_points_from_round_to_allpoints();

                if (total_scores[player_sp->get_position()] != player_sp->get_all_points())
                {
                    throw std::runtime_error("Total points are not equal to the points from the round");
                }

                got_total = true;
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
                continue;
            }
        }
        else if (packet_name == "WRONG")
        {
            ingame_comm_wrappers::WRONG_Wrapper wrong;
            cardCls::Lewa lewa(curr_lewa_id);
            wrong.read(socket_fd, lewa, curr_lewa_id, msg_str);
            print_log_from_read(addreses_str, packet_name, msg_str);
        }
        else 
        {
            // Wrong packet name given by server we ignore
            just_read_rest_of_wrong_packet(socket_fd, MAX_TOTAL_BUFF_SIZE, msg_str);
            print_log_from_read(addreses_str, packet_name, msg_str);
        }

    }
}

int klient_auto_func::klient_auto_main(
    AddressWrapper &server_address,
    AddressWrapper &client_address,
    int socket_fd,
    PlayerPosition chosen_position)
{
    while (true)
    {
        std::string packet_name;
        std::string msg_str;
        std::string addreses_str = 
        communication_addresses_to_str(server_address.get_address(),
                                        client_address.get_address(), 
                                        false, NOT_INVOKED_BY_SERVER);
        int ret_val_read_name = handle_read_packet_name(socket_fd, 
                                                packet_name, 
                                                server_address.get_address(), 
                                                client_address.get_address(), 
                                                INIT_PACKET_NAME_SIZE, 
                                                MAX_DEAL_BUFF_SIZE);

        if (ret_val_read_name == CONTINUE)
            continue;
        else if (ret_val_read_name != SUCCESS)
            return ret_val_read_name;

        if (packet_name == "BUSY")
        {
            init_comm_wrappers::BUSY_Wrapper busy;
            std::vector<PlayerPosition> busy_positions;
            try 
            {
                int ret_busy_val = busy.read(socket_fd, busy_positions, msg_str);
                print_log_from_read(addreses_str, packet_name, msg_str);
                ret_busy_val = FAILURE;
                return ret_busy_val;
            }
            catch (std::exception &e)
            {
                print_log_from_read(addreses_str, packet_name, msg_str);
                std::cerr << e.what() << "\n";
                return FAILURE;
            }
        }
        else if (packet_name == "DEAL")
        {

            init_comm_wrappers::DEAL_Wrapper deal;
            GameType game_type;
            PlayerPosition first_player_pos;
            cardCls::DeckOfCards my_hand;

            try 
            {
                int ret_val_deal = deal.read(socket_fd, game_type, first_player_pos, my_hand, msg_str); 

                print_log_from_read(addreses_str, packet_name, msg_str);

                if (ret_val_deal == FAILURE)
                {
                    continue;
                }
                else if (ret_val_deal != SUCCESS)
                {
                    return FAILURE;
                }
            }
            catch (std::exception &e)
            {
                print_log_from_read(addreses_str, packet_name, msg_str);
                std::cerr << e.what() << "\n";
                continue;
            }

            std::shared_ptr<Player> player_sp = std::make_shared<Player>(my_hand, chosen_position, game_type);
            player_sp->set_player_address(client_address);
            player_sp->set_server_address(server_address);

            if (play_game(socket_fd, player_sp) != SUCCESS)
                return FAILURE;
            return SUCCESS; 
        }
        else 
        {
            // Server sent wrong packet - we ignore it
            just_read_rest_of_wrong_packet(socket_fd, MAX_DEAL_BUFF_SIZE, msg_str);
            print_log_from_read(addreses_str, packet_name, msg_str);

            continue;
        }
    }
}
