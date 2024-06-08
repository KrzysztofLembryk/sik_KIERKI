#include "TCP_threads.h"
#include "constants.h"
#include <poll.h>
#include "polls_func.h"
#include "ingame_comm_wrappers.h"
#include "init_comm_wrappers.h"
#include "TCP_handler.h"
#include "err.h"
#include "common.h"

using GM_sp = std::shared_ptr<gm::GameMaster>;
using Player_sp = std::shared_ptr<Player>;
using BinSem_sp = std::shared_ptr<std::binary_semaphore>;

int handle_player_msg_at_wrong_time(int client_fd, uint8_t curr_round,
    Player_sp player_sp, GM_sp game_master_sp)
{
    std::string addreses_str = communication_addresses_to_str(player_sp->get_server_address(), player_sp->get_client_address(), true);
    std::string packet_name;
    int ret_code_read_name = tcp::TCP_read_packet_name(client_fd, INGAME_PACKET_NAME_SIZE, packet_name); 
    // {
    //     // end connection
    //     err_func::error("ENDING CONNECTION - GOT ERROR WHILE READING PACKET NAME");
    //     return ERROR;
    // }
    // if (packet_name != "TRICK")
    // {
    //     // end connection
    //     err_func::error("ENDING CONNECTION - GOT WRONG PACKET NAME");
    //     return ERROR;
    // }
    std::string msg_str;
    std::string final_str;
    ingame_comm_wrappers::TRICK_Wrapper client_trick;
    cardCls::Lewa lewa;

    try 
    {
        int ret_code = client_trick.read(client_fd, lewa, curr_round, msg_str);

        print_log_from_read_thread_safe(addreses_str, packet_name, msg_str, game_master_sp);

        if (ret_code != SUCCESS)
        {
            return ERROR;
        }
        if (packet_name != "TRICK")
        {
            err_func::error("ENDING CONNECTION - GOT WRONG PACKET NAME");
            return ERROR;
        }
        if (ret_code_read_name != SUCCESS)
        {
            return ERROR;
        }
    }
    catch (const std::exception &e)
    {
        // end connection
        print_log_from_read_thread_safe(addreses_str, packet_name, msg_str, game_master_sp);
        err_func::error("ENDING CONNECTION -- GOT EXCEPTION WHILE READING TRICK - sent trick packet was invalid - either value or suit was not allowed character");
        return ERROR;
    }
    // After we read trick packet, we can send WRONG msg
    addreses_str = communication_addresses_to_str(player_sp->get_server_address(), player_sp->get_client_address(), false);
    ingame_comm_wrappers::WRONG_Wrapper wrong;
    cardCls::Lewa wrong_lewa(curr_round);
    wrong.write(client_fd, wrong_lewa, msg_str);
    print_log_from_write_thread_safe(addreses_str, msg_str, game_master_sp);

    return SUCCESS;
}

int handle_DEAL(int client_fd, GM_sp game_master_sp, Player_sp player_sp, std::shared_ptr<bool> thread_ended_sp, BinSem_sp semaphore_TCP)
{
    auto p_hand = game_master_sp->get_player_deck(player_sp->get_position());
    player_sp->set_hand(p_hand);
    player_sp->set_game_type(game_master_sp->get_game_type());

    std::string msg;
    std::string address_str = communication_addresses_to_str(player_sp->get_server_address(), player_sp->get_client_address(), false);
    init_comm_wrappers::DEAL_Wrapper deal;

    try
    {
        deal.write(client_fd, game_master_sp->get_game_type(), game_master_sp->get_whose_turn(), 
        player_sp->get_hand(), msg);
        print_log_from_write_thread_safe(address_str, msg, game_master_sp);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';

        print_log_from_write_thread_safe(address_str, msg, game_master_sp);

        (*thread_ended_sp) = true;
        semaphore_TCP->release();

        return ERROR;
    }

    semaphore_TCP->release();
    return SUCCESS;
}

int handle_TRICK(int client_fd, GM_sp game_master_sp, Player_sp player_sp, std::shared_ptr<bool> thread_ended_sp, BinSem_sp semaphore_TCP)
{
    ingame_comm_wrappers::TRICK_Wrapper trick;
    cardCls::Lewa lewa_with_good_id(game_master_sp->get_curr_round_nbr());
    ingame_comm_wrappers::WRONG_Wrapper wrong;
    std::string address_str;
    std::string msg_str;
    std::string packet_name;

    while (true)
    {
        try 
        {
            address_str = communication_addresses_to_str(player_sp->get_server_address(), player_sp->get_client_address(), false);

            trick.write(client_fd, *(game_master_sp->get_curr_lewa()), msg_str);
            print_log_from_write_thread_safe(address_str, msg_str, game_master_sp);
            // trick.read() sets lewa_id and adds cards to Lewa

            address_str = communication_addresses_to_str(player_sp->get_server_address(), player_sp->get_client_address(), true);

            int ret_code_read_name = tcp::TCP_read_packet_name(client_fd, INGAME_PACKET_NAME_SIZE, packet_name); 
            cardCls::Lewa client_ret_lewa;
            int ret_code = trick.read(client_fd, client_ret_lewa, game_master_sp->get_curr_round_nbr(), msg_str);

            print_log_from_read_thread_safe(address_str, packet_name, msg_str, game_master_sp);

            if(ret_code_read_name != SUCCESS || ret_code != SUCCESS)
            {
                (*thread_ended_sp) = true;
                semaphore_TCP->release();
                return ERROR;
            }
            if (packet_name != "TRICK")
            {
                (*thread_ended_sp) = true;
                semaphore_TCP->release();
                return ERROR;
            }
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
            game_master_sp->get_curr_lewa()->size() == 0 ? 
            client_card.get_suit() : game_master_sp->get_curr_lewa()->get_cards_in_lewa()[0].get_suit();

            // We check if sent card is in players deck and if its not played
            if (player_sp->check_card_correctness(client_ret_lewa.get_cards_in_lewa()[0], bottom_card_suit) != SUCCESS)
            {
                address_str = communication_addresses_to_str(player_sp->get_server_address(), player_sp->get_client_address(), false);
                wrong.write(client_fd, lewa_with_good_id, msg_str);

                print_log_from_write_thread_safe(address_str, msg_str, game_master_sp);
                continue;
            }

            // If card is correct we add it to lewa and set that this card is 
            // played in player hand
            game_master_sp->add_card_to_lewa(client_card);
            player_sp->set_card_played(client_card);
            semaphore_TCP->release();
            return SUCCESS;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';

            (*thread_ended_sp) = true;
            semaphore_TCP->release();

            return ERROR;
        }

    }
    return SUCCESS;
}

int handle_TAKEN(int client_fd, GM_sp game_master_sp, Player_sp player_sp, std::shared_ptr<bool> thread_ended_sp, BinSem_sp semaphore_TCP)
{
    ingame_comm_wrappers::TAKEN_Wrapper taken;
    cardCls::Lewa curr_lewa = *(game_master_sp->get_curr_lewa());
    std::string address_str = communication_addresses_to_str(
                                            player_sp->get_server_address(), 
                                            player_sp->get_client_address(), 
                                            false);
    std::string msg_str;
    try 
    {
        // we dont need to pushback curr lewa to lewas_vec in game_master since 
        // its done when we prepare new lewa
        if (player_sp->get_position() == game_master_sp->get_who_won_lewa())
        {
            player_sp->add_points_in_curr_round(curr_lewa);
        }
        taken.write(client_fd, curr_lewa, game_master_sp->get_who_won_lewa(), msg_str);

        print_log_from_write_thread_safe(address_str, msg_str, game_master_sp);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';

        (*thread_ended_sp) = true;
        semaphore_TCP->release();
        print_log_from_write_thread_safe(address_str, msg_str, game_master_sp);

        return ERROR;
    }

    semaphore_TCP->release();
    return SUCCESS;
}

int handle_SCORE(int client_fd, GM_sp game_master_sp, Player_sp player_sp, std::shared_ptr<bool> thread_ended_sp, BinSem_sp semaphore_TCP)
{
    ingame_comm_wrappers::SCORE_Wrapper score;
    std::string address_str = communication_addresses_to_str(
                                            player_sp->get_server_address(), 
                                            player_sp->get_client_address(), 
                                            false);
    std::string msg_str;
    try 
    {
        score.write(client_fd, game_master_sp->get_player_scores(), msg_str);
        print_log_from_write_thread_safe(address_str, msg_str, game_master_sp);
        player_sp->add_points_from_round_to_allpoints();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        print_log_from_write_thread_safe(address_str, msg_str, game_master_sp);

        (*thread_ended_sp) = true;
        semaphore_TCP->release();

        return ERROR;
    }

    semaphore_TCP->release();
    return SUCCESS;
}

int handle_TOTAL(int client_fd, GM_sp game_master_sp, Player_sp player_sp, std::shared_ptr<bool> thread_ended_sp, BinSem_sp semaphore_TCP)
{
    ingame_comm_wrappers::TOTAL_Wrapper total;
    std::string address_str = communication_addresses_to_str(
                                            player_sp->get_server_address(), 
                                            player_sp->get_client_address(), 
                                            false);
    std::string msg_str;

    try 
    {
        total.write(client_fd, game_master_sp->get_player_all_points(), msg_str);
        print_log_from_write_thread_safe(address_str, msg_str, game_master_sp);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        print_log_from_write_thread_safe(address_str, msg_str, game_master_sp);

        (*thread_ended_sp) = true;
        semaphore_TCP->release();

        return ERROR;
    }

    semaphore_TCP->release();
    return SUCCESS;
}

void TCP_threads::TCPThread::TCP_thread_main(
    std::shared_ptr<ClientFdWrapper> client_fd_sp,
    GM_sp game_master_sp,
    Player_sp player_sp,
    BinSem_sp semaphore_TCP,
    int parent_pipe_read_fd,
    std::shared_ptr<bool> thread_ended_sp)
{

    struct pollfd poll_descriptors[POLLS_NBR_OF_DSCR];
    int poll_status;

    // We wait for event on socket_fd and pipe_fd.
    poll_descriptors[TCP_SOCKET_POLLS_ID].fd = client_fd_sp->to_int();
    poll_descriptors[TCP_SOCKET_POLLS_ID].events = POLLIN;
    poll_descriptors[PIPE_POLLS_ID].fd = parent_pipe_read_fd;
    poll_descriptors[PIPE_POLLS_ID].events = POLLIN;

    while (true)
    {
        // If client disconnects, we signalize parent thread that client
        // disconnected when he shouldnt and end this thread
        if (polls_func::handle_polls_waiting(poll_status, poll_descriptors) == DISCONNECTED)
        {
            (*thread_ended_sp) = true;
            semaphore_TCP->release();
            return;
        }

        if (poll_status > 0)
        {
            // First we check if we got END signal from threads, and if not
            // then we can check if we got any new connections.
            if (poll_descriptors[PIPE_POLLS_ID].revents & POLLIN)
            {
                std::string msg;
                polls_func::handle_polls_read(parent_pipe_read_fd, msg, true);
                if (msg == "DEAL")
                {
                    if (handle_DEAL(client_fd_sp->to_int(), game_master_sp, player_sp, thread_ended_sp, semaphore_TCP) != SUCCESS)
                    {
                        return;
                    }
                }
                else if (msg == "TRICK")
                {
                    if (handle_TRICK(client_fd_sp->to_int(), game_master_sp, player_sp, thread_ended_sp, semaphore_TCP) != SUCCESS)
                    {
                        return;
                    }
                }
                else if (msg == "TAKEN")
                {
                    if (handle_TAKEN(client_fd_sp->to_int(), game_master_sp, player_sp, thread_ended_sp, semaphore_TCP) != SUCCESS)
                    {
                        return;
                    }
                }
                else if (msg == "SCORE")
                {
                    if (handle_SCORE(client_fd_sp->to_int(), game_master_sp, player_sp, thread_ended_sp, semaphore_TCP) != SUCCESS)
                    {
                        return;
                    }
                }
                else if (msg == "TOTAL")
                {
                    if (handle_TOTAL(client_fd_sp->to_int(), game_master_sp, player_sp, thread_ended_sp, semaphore_TCP) != SUCCESS)
                    {
                        return;
                    }
                }
                else if (msg == "END")
                {
                    semaphore_TCP->release();
                    return;
                }
                else
                {
                    exception_wrappers::runtime_err_wrapper("Got wrong msg from parent thread");
                }
                
            }
            if (poll_descriptors[TCP_SOCKET_POLLS_ID].revents & POLLIN)
            {
                if (handle_player_msg_at_wrong_time(
                                    client_fd_sp->to_int(), 
                                    game_master_sp->get_curr_round_nbr(), 
                                    player_sp,
                                    game_master_sp) != SUCCESS)
                {
                    (*thread_ended_sp) = true;
                    semaphore_TCP->release();
                    return;
                }
            }
        }
    }
}