#include "client_interface_lib.h"
#include <poll.h>
#include "constants.h"
#include <unistd.h>
#include "polls_func.h"
#include "exception_wrappers.h"
#include <string>
#include <cstring>
#include <regex>
#include <algorithm>
#include "common.h"
#include <vector>

void init_polls(int parent_read_dscr, struct pollfd poll_descriptors[POLLS_NBR_OF_DSCR])
{
    // We wait for event on socket_fd and pipe_fd.
    poll_descriptors[STDIN_POLLS_ID].fd = STDIN_FILENO;
    poll_descriptors[STDIN_POLLS_ID].events = POLLIN;
    poll_descriptors[PIPE_POLLS_ID].fd = parent_read_dscr;
    poll_descriptors[PIPE_POLLS_ID].events = POLLIN;
}

void read_msg_from_parent(int parent_pipe_read_fd, std::string &parent_msg)
{
    char buff[INGAME_PACKET_NAME_SIZE];
    std::memset(buff, 0, INGAME_PACKET_NAME_SIZE);

    ssize_t read_len = read(parent_pipe_read_fd, buff, INGAME_PACKET_NAME_SIZE);
    if (read_len < 0)
    {
        exception_wrappers::runtime_err_wrapper("read < 0 when reading msg from parent thread");
    }

    parent_msg = std::string(buff, read_len);
}

cardCls::CardClassWrapper create_card_from_input(std::string &input)
{
    std::string trimmed_input = input.substr(1);
    Suit suit;
    uint8_t value;

    if (trimmed_input.size() == 2)
    {
        std::vector<char> vec{trimmed_input[0]};
        value = determine_value(vec);
        suit = determine_suit(trimmed_input[1]);
    }
    else if (trimmed_input.size() == 3)
    {
        std::vector<char> vec{trimmed_input[0], trimmed_input[1]};
        value = determine_value(vec);
        suit = determine_suit(trimmed_input[2]);
    }
    else
    {
        exception_wrappers::runtime_err_wrapper("trimmed Input size is not correct");
    }

    return cardCls::CardClassWrapper(suit, value);
}

int check_card_correctness(std::string &input, std::shared_ptr<Player> player_sp)
{
    std::regex card_pattern("!([2-9]|10|[JQKA])([HDCS])");

    if (std::regex_match(input, card_pattern))
    {
        auto card = create_card_from_input(input);
        Suit bottom_card_suit = player_sp->get_curr_lewa_bottom_suit();

        if (player_sp->check_card_correctness(card, bottom_card_suit) != SUCCESS)
        {
            return ERROR;
        }
        else
        {
            player_sp->set_chosen_card_by_human_player(card);
            return SUCCESS;
        }
    }
    else
    {
        // The input does not match the pattern
        return ERROR;
    }
}

void client_interface_lib::InterfaceThread::interface_thread_main(
    std::shared_ptr<Player> player_sp,
    std::shared_ptr<std::binary_semaphore> TCP_sem,
    int parent_pipe_read_fd,
    int parent_pipe_write_fd,
    std::shared_ptr<bool> thread_ended_sp)
{
    std::string input;

    struct pollfd poll_descriptors[POLLS_NBR_OF_DSCR];
    int poll_status;
    init_polls(parent_pipe_read_fd, poll_descriptors);

    while (true)
    {
        if (polls_func::handle_polls_waiting(poll_status, poll_descriptors) == DISCONNECTED)
        {
            return;
        }
        if (poll_status > 0)
        {
            // First priority is to check if we got any message from parent
            // thread.
            if (poll_descriptors[PIPE_POLLS_ID].revents & POLLIN)
            {
                std::string parent_msg;
                read_msg_from_parent(parent_pipe_read_fd, parent_msg);

                if (parent_msg == "TRICK")
                {
                    bool card_ok_to_be_played = false;

                    while (!card_ok_to_be_played)
                    {
                        std::getline(std::cin, input);
                        if (check_card_correctness(input, player_sp) != SUCCESS)
                        {
                            std::cerr << "ERROR: Chosen card is not correct. Try again.\n";
                        }
                        else
                        {
                            card_ok_to_be_played = true;
                            TCP_sem->release();
                        }
                    }
                }
                else
                {
                    // any other msg means we end
                    return;
                }
            }
            if (poll_descriptors[STDIN_POLLS_ID].revents & POLLIN)
            {
                std::getline(std::cin, input);
                if (input == "cards")
                {
                    player_sp->print_available_cards();
                }
                else if (input == "tricks")
                {
                    player_sp->print_taken_lewas();
                }
                else if (input == "exit")
                {
                    char buff[] = "EXIT";
                    ssize_t write_len = write(parent_pipe_write_fd, buff, 4);
                    return;
                }
                else if (input == "help")
                {
                    std::cout << "Available commands:\n"
                                 "cards - show available cards on your hand\n"
                                 "tricks - show taken lewas\n"
                                 "exit - exit the game\n";
                }
                else
                {
                    std::cerr << "Unknown command. Type 'help' for available commands.\n";
                }
            }
        }
    }
}