#include "klient_auto_lib.h"
#include "init_comm_wrappers.h"
#include "ingame_comm_wrappers.h"
#include "TCP_handler.h"
#include <string>
#include "common.h"
#include <memory>
#include "player_class.h"

void just_read_rest_of_wrong_packet(int socket_fd, size_t data_size)
{
    char *buff = new char[data_size];
    ssize_t total_bytes_read = 0;

    tcp::TCP_read_till_newline(socket_fd, buff, data_size, total_bytes_read);

    delete[] buff;
}

int handle_read_packet_name(int socket_fd, std::string &packet_name, 
struct sockaddr server_address, struct sockaddr client_address, 
size_t packet_name_size, size_t data_size)
{
    ssize_t read_length;

    int ret_val_read_name = tcp::TCP_read_packet_name(socket_fd, packet_name_size, packet_name);

    if (ret_val_read_name == DISCONNECTED)
    {
        return DISCONNECTED;
    }
    if (ret_val_read_name == FAILURE)
    {
        print_communication_addresses(server_address, client_address, false);
        std::cout.write(packet_name.data(), packet_name.size());
        // Server sent wrong packet - we ignore it
        just_read_rest_of_wrong_packet(socket_fd, data_size);
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
    uint8_t curr_round = 1;
    while (true)
    {
        std::string packet_name;

        int ret_val_read_name = handle_read_packet_name(socket_fd, packet_name, player_sp->get_server_address(), player_sp->get_client_address(), 
        INGAME_PACKET_NAME_SIZE, MAX_TOTAL_BUFF_SIZE);
        if (ret_val_read_name == CONTINUE)
            continue;
        else if (ret_val_read_name != SUCCESS)
            return ret_val_read_name;
        
        if (packet_name == "TRICK")
        {
            ingame_comm_wrappers::TRICK_Wrapper trick;
            cardCls::Lewa lewa;
            try 
            {
                int ret_val_trick = trick.read(socket_fd, lewa, curr_round);
                if (ret_val_trick == FAILURE)
                {
                    continue;
                }
                else if (ret_val_trick != SUCCESS)
                {
                    return FAILURE;
                }
            }
            catch (std::exception &e)
            {
                std::cerr << e.what() << "\n";
                continue;
            }
        }
        else if (packet_name == "TAKEN")
        {

        }
        else if (packet_name == "SCORE")
        {

        }
        else if (packet_name == "TOTAL")
        {

        }
        else 
        {
            // Wrong packet name given by server we ignore
            print_communication_addresses(player_sp->get_server_address(), player_sp->get_client_address(), false);
            std::cout.write(packet_name.data(), packet_name.size());
            just_read_rest_of_wrong_packet(socket_fd, MAX_TOTAL_BUFF_SIZE);
        }

    }
}

int klient_auto_func::klient_auto_main(
    struct sockaddr &server_address,
    struct sockaddr &client_address,
    int socket_fd,
    PlayerPosition chosen_position)
{
    while (true)
    {
        std::string packet_name;

        int ret_val_read_name = handle_read_packet_name(socket_fd, packet_name, server_address, client_address, INIT_PACKET_NAME_SIZE, MAX_DEAL_BUFF_SIZE);
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
                print_communication_addresses(server_address, client_address, false);
                std::cout.write(packet_name.data(), packet_name.size());

                int ret_busy_val = busy.read(socket_fd, busy_positions);
                ret_busy_val = FAILURE;
                return ret_busy_val;
            }
            catch (std::exception &e)
            {
                std::cerr << e.what() << "\n";
                return FAILURE;
            }
        }
        else if (packet_name == "DEAL")
        {
            print_communication_addresses(server_address, client_address, false);
            std::cout.write(packet_name.data(), packet_name.size());

            init_comm_wrappers::DEAL_Wrapper deal;
            GameType game_type;
            PlayerPosition first_player_pos;
            cardCls::DeckOfCards my_hand;

            try 
            {
                int ret_val_deal = deal.read(socket_fd, game_type, first_player_pos, my_hand); 
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
                std::cerr << e.what() << "\n";
                continue;
            }

            std::shared_ptr<Player> player_sp = std::make_shared<Player>(my_hand, chosen_position, game_type, server_address);
            player_sp->set_player_address(client_address);

            if (play_game(socket_fd, player_sp) != SUCCESS)
                return FAILURE;
            return SUCCESS; 
        }
        else 
        {
            // Server sent wrong packet - we ignore it
            print_communication_addresses(server_address, client_address, false);
            std::cout.write(packet_name.data(), packet_name.size());
            just_read_rest_of_wrong_packet(socket_fd, MAX_DEAL_BUFF_SIZE);

            continue;
        }
    }
}
