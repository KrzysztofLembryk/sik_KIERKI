#include "init_comm_wrappers.h"
#include "common.h"
#include "err.h"
#include "exception_wrappers.h"
#include <cstring>
#include <iostream>
#include "TCP_handler.h"
#include "constants.h"

// IAM_Wrapper impl

/**
 * Function transforms PlayerPosition to correct char value and sends 
 * struct IAM by tcp socket
*/
void init_comm_wrappers::IAM_Wrapper::write(int socket_fd, PlayerPosition position)
{
    std::vector<char> msg_vec(name);
    msg_vec.push_back(playerPos_to_char(position));
    msg_vec.insert(msg_vec.end(), end_chars.begin(), end_chars.end());
    tcp::TCP_send_packet(socket_fd, msg_vec.data(), msg_vec.size());    

    std::cout.write(msg_vec.data(), msg_vec.size());
}

/**
 * Function reads struct IAM from tcp socket, checks if nbr of bytes read is 
 * correct, checks if packet name is correct and then transforms char position 
 * value to PlayerPosition enum, and checks if it is correct
*/
int init_comm_wrappers::IAM_Wrapper::read(int socket_fd, PlayerPosition &position)
{
    ssize_t read_length = 0;
    char read_buff[MAX_IAM_BUFF_SIZE]; 
    std::memset(read_buff, 0, MAX_IAM_BUFF_SIZE); 
    
    if (tcp::TCP_read_till_newline(socket_fd, read_buff, MAX_IAM_BUFF_SIZE, read_length) != SUCCESS)
    {
        return ERROR;
    }

    if (read_length != MAX_IAM_BUFF_SIZE)
    {
        err_func::error("read_length != IAM_BUFF_SIZE");
        return ERROR;
    }
    if (strncmp(read_buff, "IAM", 3) != 0)
    {
        err_func::error("packet name not equal 'IAM'");
        return ERROR;
    }

    position = char_to_playerPos(read_buff[3]);

    if (position == PlayerPosition::NONE_POS)
    {
        return ERROR;
    }

    return SUCCESS;
}

// BUSY_Wrapper

void init_comm_wrappers::BUSY_Wrapper::write(int socket_fd, std::vector<PlayerPosition> taken_positions)
{
    std::vector<char> msg_vec(name);

    // This should never HAPPEN if everything works correctly
    if (taken_positions.size() == 0)
    {
        msg_vec.insert(msg_vec.end(), end_chars.begin(), end_chars.end());
        tcp::TCP_send_packet(socket_fd, msg_vec.data(), msg_vec.size());
        return;
    }

    if (taken_positions.size() > MAX_PLAYERS)
        exception_wrappers::invalid_arg_wrapper("Too many taken positions");

    for (size_t i = 0; i < taken_positions.size(); i++)
        msg_vec.push_back(playerPos_to_char(taken_positions[i]));

    msg_vec.insert(msg_vec.end(), end_chars.begin(), end_chars.end());

    tcp::TCP_send_packet(socket_fd, msg_vec.data(), msg_vec.size());

    std::cout.write(msg_vec.data(), msg_vec.size());
}

/**
 *  We assume that first four bytes are read by function that called us, since
 * calling us means that these bytes were equal to BUSY
*/
int init_comm_wrappers::BUSY_Wrapper::read(
    int socket_fd, std::vector<PlayerPosition> &taken_positions)
{
    ssize_t read_length;
    char read_buff[MAX_BUSY_BUFF_SIZE]; 
    std::memset(read_buff, 0, MAX_BUSY_BUFF_SIZE); 

    // BUSY_BUFF_SIZE is maximally equal to 10 - 4 bytes for packet name = BUSY
    // 2 bytes for end chars and maximally 4 bytes for player positions thus
    // we can read maximally BUSY_BUFF_SIZE - this->name.size() bytes
    if (tcp::TCP_read_till_newline(socket_fd, read_buff, MAX_BUSY_BUFF_SIZE, read_length) != SUCCESS)
    {
        return ERROR;
    }

    if ((size_t)read_length < MIN_BUSY_BUFF_SIZE)
    {
        // Msg was BUSY\r\n thus no positions were taken
        err_func::error(" read_length < MIN_BUSY_BUFF_SIZE");
        return ERROR;
    }
    else 
    {
        for (size_t i = 0; i < (size_t)(read_length - 2); i++)
        {
            PlayerPosition pos = char_to_playerPos(read_buff[i]);

            if (pos == PlayerPosition::NONE_POS)
            {
                err_func::error("Read wrong position");
                return ERROR;
            }

            taken_positions.push_back(pos);
        }
        return SUCCESS;
    }

}

// DEAL_Wrapper

void init_comm_wrappers::DEAL_Wrapper::write(int socket_fd, GameType game_type , PlayerPosition first_player_pos, cardCls::DeckOfCards &&deck_of_cards)
{
    std::vector<char> msg_vec(name);
    std::vector<char> game_type_and_first_player_pos;

    std::cout << "DEAL write, gametype: " << (unsigned)game_type << "\n";
    game_type_and_first_player_pos.push_back(gameType_to_char(game_type));
    game_type_and_first_player_pos.push_back(playerPos_to_char(first_player_pos));

    msg_vec.insert(msg_vec.end(), game_type_and_first_player_pos.begin(), game_type_and_first_player_pos.end());

    std::vector<char> deck = deck_of_cards.to_char_vector();

    msg_vec.insert(msg_vec.end(), deck.begin(), deck.end());
    msg_vec.insert(msg_vec.end(), end_chars.begin(), end_chars.end());

    tcp::TCP_send_packet(socket_fd, msg_vec.data(), msg_vec.size());
    std::cout.write(msg_vec.data(), msg_vec.size());
}

int init_comm_wrappers::DEAL_Wrapper::read(int socket_fd, GameType &game_type, PlayerPosition &first_player_pos, cardCls::DeckOfCards &deck_of_cards)
{
    ssize_t read_length;
    char read_buff[MAX_DEAL_BUFF_SIZE]; 
    std::memset(read_buff, 0, MAX_DEAL_BUFF_SIZE);

    // DEAL_BUFF_SIZE is maximally equal to 50 - 4 bytes for packet name = DEAL
    // 2 bytes for end chars and maximally 44 bytes for deck of cards thus
    // we can read maximally DEAL_BUFF_SIZE - this->name.size() bytes
    int ret_code = tcp::TCP_read_till_newline(socket_fd, read_buff, MAX_DEAL_BUFF_SIZE, read_length);
    if (ret_code != SUCCESS)
    {
        return ret_code;
    }

    if ((size_t)read_length < MIN_DEAL_BUFF_SIZE)
    {
        err_func::error(" read_length < MIN_DEAL_BUFF_SIZE");
        return FAILURE;
    }

    game_type = char_to_gameType(read_buff[0]);
    first_player_pos = char_to_playerPos(read_buff[1]);

    uint8_t value;
    Suit suit;
    std::vector<char> char_val_vec;
    size_t i = 2;

    while (i < (size_t)read_length - 3)
    {
        if (read_buff[i] == '1')
        {
            char_val_vec.push_back(read_buff[i]);
            char_val_vec.push_back(read_buff[i + 1]);
            suit = determine_suit(read_buff[i + 2]);
            i += 3;
        }
        else 
        {
            char_val_vec.push_back(read_buff[i]);
            suit = determine_suit(read_buff[i + 1]);
            i += 2;
        }
        value = determine_value(char_val_vec);

        deck_of_cards.add_card(cardCls::CardClassWrapper(suit, value));
        char_val_vec.clear();
    }

    if (deck_of_cards.size() != 13)
    {
        err_func::error("Deck of cards size is not equal to 13");
        return FAILURE;
    }

    return SUCCESS;
}
