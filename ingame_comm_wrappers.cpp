#include "ingame_comm_wrappers.h"
#include "common.h"
#include "TCP_handler.h"
#include "constants.h"
#include "err.h"
#include <endian.h>

// We check id of lewa, it can be from '1' to '13' thus thanks to curr_round
// variable we know how many bytes we need to read, set_lewa_id method 
// checks if read lewa id is in correct range
void determine_lewa_id(size_t &i, const char *read_buff, cardCls::Lewa &lewa, uint8_t curr_round)
{
    if (curr_round >= 10)
    {
        lewa.set_lewa_id({read_buff[0], read_buff[1]});
        i = 2;
    }
    else
    {
        lewa.set_lewa_id(std::vector<char>{read_buff[0]});
        i = 1;
    }
}

void parse_char_and_add_card_to_lewa(char *read_buff, 
                                        size_t &i, 
                                        cardCls::Lewa &lewa, 
                                        std::vector<char> &char_val_vec, 
                                        uint8_t &value, 
                                        Suit &suit)
{
    if (read_buff[i] == '1')
    {
        char_val_vec.push_back(read_buff[i]);
        char_val_vec.push_back(read_buff[i + 1]);
        value = determine_value(char_val_vec);
        suit = determine_suit(read_buff[i + 2]);
        i += 3;
    }
    else 
    {
        char_val_vec.push_back(read_buff[i]);
        suit = determine_suit(read_buff[i + 1]);
        i += 2;
    }

    lewa.add_card(cardCls::CardClassWrapper(suit, value));
    char_val_vec.clear();
}

// TRICK_Wrapper impl
void ingame_comm_wrappers::TRICK_Wrapper::write(int socket_fd, 
    cardCls::Lewa  &lewa)
{
    std::vector<char> msg_vec(name);
    std::vector<char> lewa_id = lewa.get_lewa_id_as_char(); 
    std::vector<char> lewa_vec = lewa.to_char_vector();

    msg_vec.insert(msg_vec.end(), lewa_id.begin(), lewa_id.end());
    msg_vec.insert(msg_vec.end(), lewa_vec.begin(), lewa_vec.end());
    msg_vec.insert(msg_vec.end(), end_chars.begin(), end_chars.end());

    tcp::TCP_send_packet(socket_fd, msg_vec.data(), msg_vec.size());
}


int ingame_comm_wrappers::TRICK_Wrapper::read(int socket_fd, 
    cardCls::Lewa &lewa, uint8_t curr_round)
{
    ssize_t read_length;
    char read_buff[MAX_TRICK_BUFF_SIZE];
    std::memset(read_buff, 0, MAX_TRICK_BUFF_SIZE); 

    if (tcp::TCP_read_till_newline(socket_fd, read_buff, MAX_TRICK_BUFF_SIZE, read_length) != SUCCESS)
    {
        return ERROR;
    }

    if (read_length < MIN_TRICK_BUFF_SIZE)
    {
        err_func::error(" read_length < MIN_TRICK_BUFF_SIZE");
        return ERROR;
    }

    uint8_t value;
    Suit suit;
    std::vector<char> char_val_vec;
    size_t i;

    determine_lewa_id(i, read_buff, lewa, curr_round);
    // We can substract - 3 from read_length since we know that read_length is 
    // >= MIN_TRICK_BUFF_SIZE == 3
    while (i < (size_t)read_length - 2)
    {
        parse_char_and_add_card_to_lewa(read_buff, i, lewa, char_val_vec, value, suit);
    }

    if (lewa.size() > MAX_LEWA_SIZE)
    {
        err_func::error("Lewa size > 4");
        return ERROR;
    }
    return SUCCESS;
}

// WRONG_Wrapper impl

void ingame_comm_wrappers::WRONG_Wrapper::write(int socket_fd, const cardCls::Lewa &lewa)
{
    std::vector<char> msg_vec(name);
    std::vector<char> lewa_id = lewa.get_lewa_id_as_char(); 
    
    msg_vec.insert(msg_vec.end(), lewa_id.begin(), lewa_id.end());
    msg_vec.insert(msg_vec.end(), end_chars.begin(), end_chars.end());

    tcp::TCP_send_packet(socket_fd, msg_vec.data(), msg_vec.size());
}

int ingame_comm_wrappers::WRONG_Wrapper::read(int socket_fd, cardCls::Lewa &lewa, uint8_t curr_round)
{
    ssize_t read_length;
    char read_buff[MAX_WRONG_BUFF_SIZE];
    std::memset(read_buff, 0, MAX_WRONG_BUFF_SIZE);

    if (tcp::TCP_read_till_newline(socket_fd, read_buff, MAX_WRONG_BUFF_SIZE, read_length) != SUCCESS)
    {
        return ERROR;
    }

    if (read_length < MIN_WRONG_BUFF_SIZE)
    {
        err_func::error(" read_length < MIN_WRONG_BUFF_SIZE");
        return ERROR;
    }

    size_t i;
    determine_lewa_id(i, read_buff, lewa, curr_round);

    return SUCCESS;
}

// TAKEN_Wrapper impl

void ingame_comm_wrappers::TAKEN_Wrapper::write(int socket_fd, 
                                    const cardCls::Lewa &lewa, 
                                    const PlayerPosition &player_who_took_lewa)
{
    std::vector<char> msg_vec(name);
    std::vector<char> lewa_vec = lewa.to_char_vector();
    std::vector<char> lewa_id = lewa.get_lewa_id_as_char();
    
    msg_vec.insert(msg_vec.end(), lewa_id.begin(), lewa_id.end());
    msg_vec.insert(msg_vec.end(), lewa_vec.begin(), lewa_vec.end());
    msg_vec.push_back(playerPos_to_char(player_who_took_lewa));
    msg_vec.insert(msg_vec.end(), end_chars.begin(), end_chars.end());

    tcp::TCP_send_packet(socket_fd, msg_vec.data(), msg_vec.size());
}

int ingame_comm_wrappers::TAKEN_Wrapper::read(int socket_fd, 
                                    cardCls::Lewa &lewa, 
                                    PlayerPosition &player_who_took_lewa,
                                    uint8_t curr_round)
{
    ssize_t read_length;
    char read_buff[MAX_TAKEN_BUFF_SIZE];
    std::memset(read_buff, 0, MAX_TAKEN_BUFF_SIZE);

    if (tcp::TCP_read_till_newline(socket_fd, read_buff, MAX_TAKEN_BUFF_SIZE, read_length) != SUCCESS)
    {
        return ERROR;
    }

    if ((size_t)read_length < MIN_TAKEN_BUFF_SIZE)
    {
        err_func::error(" read_length < MIN_TAKEN_BUFF_SIZE");
        return ERROR;
    }

    uint8_t value;
    Suit suit;
    std::vector<char> char_val_vec;
    size_t i;

    determine_lewa_id(i, read_buff, lewa, curr_round);

    while (lewa.size() < 4 && i < (size_t)read_length - 3)
    {
        parse_char_and_add_card_to_lewa(read_buff, i, lewa, char_val_vec, value, suit);
    }

    if (i != (size_t)read_length - 3)
    {
        err_func::error(" i != read_length - 3 - but it should since we should read lewa_id and 4 cards in lewa");
        return ERROR;
    }

    player_who_took_lewa = char_to_playerPos(read_buff[read_length - 3]);

    return SUCCESS;
}

// SCORE_Wrapper impl

void ingame_comm_wrappers::SCORE_Wrapper::write(int socket_fd, const std::map<PlayerPosition, uint8_t> &scores)
{
    std::vector<char> msg_vec(name);

    for (const auto &score : scores)
    {
        msg_vec.push_back(playerPos_to_char(score.first));
        msg_vec.push_back(static_cast<char>(score.second));
    }
    msg_vec.insert(msg_vec.end(), end_chars.begin(), end_chars.end());

    tcp::TCP_send_packet(socket_fd, msg_vec.data(), msg_vec.size());
}

int ingame_comm_wrappers::SCORE_Wrapper::read(int socket_fd, std::map<PlayerPosition, uint8_t> &scores)
{
    ssize_t read_length;
    char read_buff[MAX_SCORE_BUFF_SIZE - this->name.size()];
    std::memset(read_buff, 0, MAX_SCORE_BUFF_SIZE - this->name.size());

    if (tcp::TCP_read_till_newline(socket_fd, read_buff, MAX_SCORE_BUFF_SIZE - this->name.size(), read_length) != SUCCESS)
    {
        return ERROR;
    }

    if ((size_t)read_length != MAX_SCORE_BUFF_SIZE - this->name.size())
    {
        err_func::error(" read_length != 10 -- 10 bytes are required to be sent");
        return ERROR;
    }

    for (size_t i = 0; i < (size_t)(read_length - 2); i+=2)
    {
        PlayerPosition pos = char_to_playerPos(read_buff[i]);
        uint8_t score = static_cast<uint8_t>(read_buff[i + 1]);

        scores[pos] = score;
    }

    return SUCCESS;
}

void ingame_comm_wrappers::TOTAL_Wrapper::write(int socket_fd, 
    const std::map<PlayerPosition, uint16_t> &total_scores)
{
    std::vector<char> msg_vec(name);

    for (const auto &score : total_scores)
    {
        msg_vec.push_back(playerPos_to_char(score.first));

        // We need to convert score to big endian and then send it
        uint16_t score_val_be = htobe16(score.second);
        char hi = score_val_be >> 8;
        char lo = score_val_be & 0xFF;

        msg_vec.push_back(hi);
        msg_vec.push_back(lo);
    }
    msg_vec.insert(msg_vec.end(), end_chars.begin(), end_chars.end());

    tcp::TCP_send_packet(socket_fd, msg_vec.data(), msg_vec.size());
}

int ingame_comm_wrappers::TOTAL_Wrapper::read(int socket_fd, std::map<PlayerPosition, uint16_t> &total_scores)
{
    ssize_t read_length;
    char read_buff[MAX_TOTAL_BUFF_SIZE - this->name.size()];
    std::memset(read_buff, 0, MAX_TOTAL_BUFF_SIZE - this->name.size());

    if (tcp::TCP_read_till_newline(socket_fd, read_buff, MAX_TOTAL_BUFF_SIZE - this->name.size(), read_length) != SUCCESS)
    {
        return ERROR;
    }

    if ((size_t)read_length != MAX_TOTAL_BUFF_SIZE - this->name.size())
    {
        err_func::error(" read_length != 14 -- 14 bytes are required to be sent");
        return ERROR;
    }

    for (size_t i = 0; i < (size_t)(read_length - 2); i+=3)
    {
        PlayerPosition pos = char_to_playerPos(read_buff[i]);

        uint8_t hi = static_cast<uint8_t>(read_buff[i + 1]);
        uint8_t lo = static_cast<uint8_t>(read_buff[i + 2]);
        uint16_t score = (uint16_t)hi | (uint16_t)lo << 8;

        total_scores[pos] = score;
    }

    return SUCCESS;
}