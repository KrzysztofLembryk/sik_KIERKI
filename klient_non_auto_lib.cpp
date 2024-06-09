#include "klient_non_auto_lib.h"
#include <poll.h>
#include "constants.h"
#include "exception_wrappers.h"
#include <iostream>
#include <unistd.h>
#include "polls_func.h"
#include "client_interface_lib.h"
#include <thread>
#include "TCP_handler.h"
#include "common.h"
#include "err.h"
#include "ingame_comm_wrappers.h"
#include "init_comm_wrappers.h"
#include "pretty_print_packets.h"
#include "btwn_thread_comm.h"

using BinSem_sp = std::shared_ptr<std::binary_semaphore>;

void init_polls(int socket_fd, int child_pipe_fd[2], struct pollfd poll_descriptors[POLLS_NBR_OF_DSCR])
{
    // We wait for event on socket_fd and pipe_fd.
    poll_descriptors[TCP_SOCKET_POLLS_ID].fd = socket_fd;
    poll_descriptors[TCP_SOCKET_POLLS_ID].events = POLLIN;
    poll_descriptors[PIPE_POLLS_ID].fd = child_pipe_fd[PIPE_READ_DSCR];
    poll_descriptors[PIPE_POLLS_ID].events = POLLIN;
}

void just_read_rest_of_wrong_packet(int socket_fd, size_t data_size, std::string &msg_str)
{
    char *buff = new char[data_size];
    ssize_t total_bytes_read = 0;

    tcp::TCP_read_till_newline(socket_fd, buff, data_size, total_bytes_read, msg_str);

    delete[] buff;
}

int read_first_char_of_packet_name(int socket_fd, std::string &msg)
{
    char curr_char;
    ssize_t read_length = readn(socket_fd, &curr_char, 1);

    if (read_length < 0)
    {
        if (errno == EAGAIN)
        {
            err_func::error("readn < 0 --> readn timeout");
            return TIMEOUT;
        }
        else
        {
            // exception_wrappers::runtime_err_wrapper("readn < 0");
            err_func::error("readn < 0");
            return ERROR;
        }
    }
    else if (read_length == 0)
    {
        // exception_wrappers::runtime_err_wrapper("read_len == 0 -- no newline found in packet name or sent packet is to short or connection was closed");
        exception_wrappers::runtime_err_wrapper(" - connection closed read_len == 0");
    }
    msg = std::string(&curr_char, 1);
    return SUCCESS;
}

int read_packet_name(int socket_fd, std::string &packet_name)
{
    std::string first_letter_str;
    int ret_val_read_name = read_first_char_of_packet_name(socket_fd, first_letter_str);

    if (ret_val_read_name != SUCCESS)
    {
        return ret_val_read_name;
    }

    std::string rest;
    if (first_letter_str == "D" || first_letter_str == "B")
    {
        ret_val_read_name = tcp::TCP_read_packet_name(socket_fd, INIT_PACKET_NAME_SIZE - 1, rest);
    }
    else
    {
        ret_val_read_name = tcp::TCP_read_packet_name(socket_fd, INGAME_PACKET_NAME_SIZE - 1, rest);
    }

    packet_name = first_letter_str + rest;

    if (ret_val_read_name == DISCONNECTED)
    {
        return DISCONNECTED;
    }
    if (ret_val_read_name == FAILURE)
    {
        // Server sent wrong packet - we ignore it
        if (first_letter_str == "D" || first_letter_str == "B")
            just_read_rest_of_wrong_packet(socket_fd, MAX_DEAL_BUFF_SIZE, rest);
        else
            just_read_rest_of_wrong_packet(socket_fd, MAX_TOTAL_BUFF_SIZE, rest);
        return CONTINUE;
    }
    if (ret_val_read_name != SUCCESS)
    {
        return ERROR;
    }

    return SUCCESS;
}

int handle_server_communication(int socket_fd, std::shared_ptr<Player> player_sp, BinSem_sp semaphore_TCP, BinSem_sp sem_print, int child_write_fd)
{
    static uint8_t curr_lewa_id = 1;
    static bool got_score = false;
    static bool got_total = false;
    std::string packet_name;
    std::string msg_str;
    int ret_val_read_name = read_packet_name(socket_fd,
                                             packet_name);

    if (ret_val_read_name == CONTINUE || ret_val_read_name == TIMEOUT)
        return CONTINUE;
    else if (ret_val_read_name != SUCCESS)
        return ERROR;

    if (packet_name == "BUSY")
    {
        init_comm_wrappers::BUSY_Wrapper busy;
        std::vector<PlayerPosition> busy_positions;
        try
        {
            int ret_busy_val = busy.read(socket_fd, busy_positions, msg_str);
            pretty_packets::pretty_print_BUSY(busy_positions, sem_print);

            ret_busy_val = ERROR;
            return ret_busy_val;
        }
        catch (std::exception &e)
        {
            std::cerr << e.what() << "\n";
            return ERROR;
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

            if (ret_val_deal == FAILURE)
            {
                return CONTINUE;
            }
            else if (ret_val_deal != SUCCESS)
            {
                return ERROR;
            }

            pretty_packets::pretty_print_DEAL(game_type, first_player_pos, my_hand, sem_print);
        }
        catch (std::exception &e)
        {
            std::cerr << e.what() << "\n";
            return CONTINUE;
        }

        player_sp->set_hand(my_hand);
        player_sp->set_game_type(game_type);
        player_sp->zero_curr_SCORE();
        player_sp->clear_lewas_taken();
        curr_lewa_id = 1;
        got_score = false;
        got_total = false;

        return SUCCESS;
    }
    else if (packet_name == "TRICK")
    {
        ingame_comm_wrappers::TRICK_Wrapper trick;
        cardCls::Lewa lewa;
        try 
        {
            int ret_val_trick = trick.read(socket_fd, lewa, curr_lewa_id, msg_str);
            if (ret_val_trick == FAILURE)
            {
                return CONTINUE;
            }
            else if (ret_val_trick != SUCCESS)
            {
                return ERROR;
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
            player_sp->set_curr_lewa_bottom_suit(bottom_card_suit); 

            pretty_packets::pretty_print_TRICK(lewa, sem_print);
            btwn_thread_comm::send_msg(child_write_fd, "TRICK");
            semaphore_TCP->acquire();

            auto chosen_card_by_human = player_sp->get_chosen_card_by_human_player();

            player_sp->set_card_played(chosen_card_by_human);

            cardCls::Lewa ret_lewa(lewa.get_lewa_id());

            ret_lewa.add_card(chosen_card_by_human);
            trick.write(socket_fd, ret_lewa, msg_str);
            return SUCCESS;

        }
        catch (std::exception &e)
        {
            std::cerr << e.what() << "\n";
            return CONTINUE;
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

            if (ret_val_taken == FAILURE)
            {
                return CONTINUE;
            }
            else if (ret_val_taken != SUCCESS)
            {
                return ERROR;
            }

            if (player_sp->get_position() == player_who_took_lewa)
            {
                player_sp->add_points_in_curr_round(lewa);
                player_sp->add_lewa_to_lewas_taken(lewa);
            }

            // we set that our card in this lewa was played (we already set
            // it in TRICK, but if we reconnect to server in deal we get 
            // new hand without set cards that we played, so even though 
            // card might be already set we set it again)
            player_sp->set_card_played(lewa);
            pretty_packets::pretty_print_TAKEN(lewa, player_who_took_lewa, sem_print);
            curr_lewa_id++;
            return SUCCESS;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return CONTINUE;
        }
    }
    else if (packet_name == "WRONG")
    {
        ingame_comm_wrappers::WRONG_Wrapper wrong;
        cardCls::Lewa lewa(curr_lewa_id);
        int ret_val = wrong.read(socket_fd, lewa, curr_lewa_id, msg_str);
        if (ret_val != SUCCESS)
        {
            return ERROR;
        }
        pretty_packets::pretty_print_WRONG(lewa.get_lewa_id(), sem_print);
        return SUCCESS;
    }
    else if (packet_name == "SCORE")
    {
        ingame_comm_wrappers::SCORE_Wrapper score;
        std::map<PlayerPosition, uint8_t> scores;
        try 
        {
            int ret_val_score = score.read(socket_fd, scores, msg_str);

            if (ret_val_score == FAILURE)
            {
                return CONTINUE;
            }
            else if (ret_val_score != SUCCESS)
            {
                return ERROR;
            }
            got_score = true;

            pretty_packets::pretty_print_SCORE(scores, sem_print);
        }
        catch (std::exception &e)
        {
            std::cerr << e.what() << "\n";
            return CONTINUE;
        }
    }
    else if (packet_name == "TOTAL")
    {

        ingame_comm_wrappers::TOTAL_Wrapper total;
        std::map<PlayerPosition, uint32_t> total_scores;
        try
        {
            int ret_val_total = total.read(socket_fd, total_scores, msg_str);
            if (ret_val_total == FAILURE)
            {
                return CONTINUE;
            }
            else if (ret_val_total != SUCCESS)
            {
                return ERROR;
            }
            player_sp->add_points_from_round_to_allpoints();
            got_total = true;
            pretty_packets::pretty_print_TOTAL(total_scores, sem_print);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            return CONTINUE;
        }
    }
}

// This function will handle TCP connection with server and also getting msg
// from thread that takes care of client interface
int klient_non_auto_func::klient_non_auto_main(AddressWrapper &server_address,
                                               AddressWrapper &client_address,
                                               int socket_fd,
                                               PlayerPosition chosen_position)
{
    int child_pipe_fd[2];

    if (pipe(child_pipe_fd) == -1)
    {
        exception_wrappers::runtime_err_wrapper("Failed to create pipe");
    }
    struct pollfd poll_descriptors[POLLS_NBR_OF_DSCR];
    int poll_status;
    init_polls(socket_fd, child_pipe_fd, poll_descriptors);

    BinSem_sp semaphore_TCP = std::make_shared<std::binary_semaphore>(0);
    BinSem_sp semaphore_PRINT = std::make_shared<std::binary_semaphore>(1);
    std::shared_ptr<Player> player_sp = std::make_shared<Player>(chosen_position);

    client_interface_lib::InterfaceThread interface_thread;

    std::thread t(
        [player_sp, semaphore_TCP, semaphore_PRINT, child_pipe_fd, interface_thread]() mutable
        {
            interface_thread.interface_thread_main(player_sp,
                                                   semaphore_TCP,
                                                   semaphore_PRINT,
                                                   child_pipe_fd[PIPE_READ_DSCR],
                                                   child_pipe_fd[PIPE_WRITE_DSCR]);
        });
    t.detach();

    while (true)
    {

        if (polls_func::handle_polls_waiting(poll_status, poll_descriptors) == DISCONNECTED)
        {
            return ERROR;
        }

        if (poll_status > 0)
        {
            // First we handle TCP communication, then msgs from client
            // interface
            if (poll_descriptors[TCP_SOCKET_POLLS_ID].revents & POLLIN)
            {
                int ret_val = handle_server_communication(socket_fd, player_sp, semaphore_TCP, semaphore_PRINT, child_pipe_fd[PIPE_WRITE_DSCR]);

                if (ret_val == ERROR)
                {
                    return FAILURE;
                }
            }
            if (poll_descriptors[PIPE_POLLS_ID].revents & POLLIN)
            {
                std::string msg;
                polls_func::handle_polls_read(child_pipe_fd[PIPE_READ_DSCR], msg, true);
                if (msg == "EXIT")
                {
                    // if player ends we return failure since we end with
                    // SUCCESS only if the game ends
                    return FAILURE;
                }
                else
                {
                    exception_wrappers::runtime_err_wrapper("Got wrong msg from client thread");
                }
            }
        }
    }
}