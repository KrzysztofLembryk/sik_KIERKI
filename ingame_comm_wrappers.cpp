#include "ingame_comm_wrappers.h"
#include "common.h"
#include "TCP_handler.h"
#include "constants.h"
#include "err.h"
#include <endian.h>
#include "exception_wrappers.h"
#include <type_traits>

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

template <typename T>
void get_scores_from_buffer(ssize_t read_length, char *read_buff, std::map<PlayerPosition, T> &scores)
{
    std::map<PlayerPosition, bool> player_checked( {{N, false}, {E, false}, {S, false}, {W, false}});
    std::string score_str;
    char prev_pos = '1';
    bool at_least_one_byte_score = false;
    
    for (size_t i = 0; i < (size_t)(read_length - 2); i++)
    {
        if (read_buff[i] == 'N' || read_buff[i] == 'E' || read_buff[i] == 'S' || read_buff[i] == 'W')
        {
            if (player_checked[char_to_playerPos(read_buff[i])])
            {
                exception_wrappers::runtime_err_wrapper("Player already checked");
            }
            // We need to remember previous position to be able to parse this 
            // position score
            if (prev_pos == '1')
            {
                prev_pos = read_buff[i];
            }
            else 
            {
                if (!at_least_one_byte_score)
                {
                    exception_wrappers::invalid_arg_wrapper("Score is empty");
                }
                if (std::is_same<T, uint8_t>::value)
                {
                    uint16_t score = (uint16_t)std::stoi(score_str);
                    if (score >= 255)
                    {
                        exception_wrappers::invalid_arg_wrapper("Score > 255");
                    }

                    scores[char_to_playerPos(prev_pos)] = (T)std::stoi(score_str);
                }
                else 
                {
                    scores[char_to_playerPos(prev_pos)] = (T)std::stoul(score_str);
                }
                prev_pos = read_buff[i];
            }

            player_checked[char_to_playerPos(read_buff[i])] = true;
            score_str.clear();
            at_least_one_byte_score = false;
        }
        else
        {
            if (read_buff[i] >= '0' && read_buff[i] <= '9')
            {
                at_least_one_byte_score = true;
                score_str.push_back(read_buff[i]);
            }
            else 
            {
                exception_wrappers::invalid_arg_wrapper("Read char is not a digit but is in score");
            }
        }
    }

    // after loop ends last position score is not added so we need to do it
    if (std::is_same<T, uint8_t>::value)
    {
        uint16_t score = (uint16_t)std::stoi(score_str);
        if (score >= 255)
        {
            exception_wrappers::invalid_arg_wrapper("Score > 255");
        }

        scores[char_to_playerPos(prev_pos)] = (T)std::stoi(score_str);
    }
    else 
    {
        scores[char_to_playerPos(prev_pos)] = (T)std::stoul(score_str);
    }
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

    if ((size_t)read_length < MIN_TRICK_BUFF_SIZE)
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

    if ((size_t)read_length < MIN_WRONG_BUFF_SIZE)
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

        // We dont need to remove '\0' from vector since it is not at the end
        std::string score_str = std::to_string(score.second);
        std::vector<char> score_vec(score_str.begin(), score_str.end());

        msg_vec.insert(msg_vec.end(), score_vec.begin(), score_vec.end());
    }

    msg_vec.insert(msg_vec.end(), end_chars.begin(), end_chars.end());

    tcp::TCP_send_packet(socket_fd, msg_vec.data(), msg_vec.size());
}

int ingame_comm_wrappers::SCORE_Wrapper::read(int socket_fd, std::map<PlayerPosition, uint8_t> &scores)
{
    ssize_t read_length;
    char read_buff[MAX_SCORE_BUFF_SIZE];
    std::memset(read_buff, 0, MAX_SCORE_BUFF_SIZE );

    if (tcp::TCP_read_till_newline(socket_fd, read_buff, MAX_SCORE_BUFF_SIZE, read_length) != SUCCESS)
    {
        return ERROR;
    }
    
    if ((size_t)read_length < MIN_SCORE_BUFF_SIZE)
    {
        err_func::error(" read_length < MIN_SCORE_BUFF_SIZE");
        return ERROR;
    }

    get_scores_from_buffer(read_length, read_buff, scores);

    return SUCCESS;
}

void ingame_comm_wrappers::TOTAL_Wrapper::write(int socket_fd, 
    const std::map<PlayerPosition, uint32_t> &total_scores)
{
    std::vector<char> msg_vec(name);

    for (const auto &score : total_scores)
    {
        std::cout << "Player: " << playerPos_to_char(score.first) << " score: " << score.second << '\n';
        msg_vec.push_back(playerPos_to_char(score.first));

        // At the end there is no '\0' character so we dont need to remove it
        std::string score_str = std::to_string(score.second);
        std::vector<char> score_vec(score_str.begin(), score_str.end());

        msg_vec.insert(msg_vec.end(), score_vec.begin(), score_vec.end());
    }
    msg_vec.insert(msg_vec.end(), end_chars.begin(), end_chars.end());

    tcp::TCP_send_packet(socket_fd, msg_vec.data(), msg_vec.size());
}

int ingame_comm_wrappers::TOTAL_Wrapper::read(int socket_fd, std::map<PlayerPosition, uint32_t> &total_scores)
{
    ssize_t read_length;
    char read_buff[MAX_TOTAL_BUFF_SIZE];
    std::memset(read_buff, 0, MAX_TOTAL_BUFF_SIZE);

    if (tcp::TCP_read_till_newline(socket_fd, read_buff, MAX_TOTAL_BUFF_SIZE, read_length) != SUCCESS)
    {
        return ERROR;
    }

    if ((size_t)read_length < MIN_TOTAL_BUFF_SIZE)
    {
        err_func::error(" read_length < MIN_TOTAL_BUFF_SIZE");
        return ERROR;
    }

    get_scores_from_buffer(read_length, read_buff, total_scores); 

    return SUCCESS;
}