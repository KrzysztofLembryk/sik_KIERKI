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
#include "btwn_thread_comm.h"

void init_polls(int parent_read_dscr, struct pollfd poll_descriptors[POLLS_NBR_OF_DSCR])
{
    // We wait for event on socket_fd and pipe_fd.
    poll_descriptors[STDIN_POLLS_ID].fd = STDIN_FILENO;
    poll_descriptors[STDIN_POLLS_ID].events = POLLIN;
    poll_descriptors[PIPE_POLLS_ID].fd = parent_read_dscr;
    poll_descriptors[PIPE_POLLS_ID].events = POLLIN;
}

cardCls::CardClassWrapper create_card_from_input(std::vector<char> &input)
{
    Suit suit;
    uint8_t value;
    if (input.size() == 2)
    {
        std::cout << "value: " << input[0] << " suit: " << input[1] << "\n";
        std::vector<char> vec{input[0]};
        value = determine_value(vec);
        suit = determine_suit(input[1]);
    }
    else if (input.size() == 3)
    {
        std::vector<char> vec{input[0], input[1]};
        value = determine_value(vec);
        suit = determine_suit(input[2]);
    }
    else
    {
        std::cout << "input size: " << input.size() << "\n";
        exception_wrappers::runtime_err_wrapper("trimmed Input size is not correct");
    }
    cardCls::CardClassWrapper card(suit, value);
    return card;
}

int check_card_correctness(std::string &input, std::shared_ptr<Player> player_sp)
{
    if (input[0] != '!')
    {
        std::cout << "ERROR: Card should start with '!' character.\n";
        return ERROR;
    }
    std::vector<char> vec;
    if (input.size() > 5 || input.size() < 3)
    {
        std::cout << "ERROR: Card should have 3 or 4 characters.\n";
        return ERROR;
    }
    for (size_t i = 1; i < input.size(); i++)
    {
        vec.push_back(input[i]);
    }

    try 
    {
        auto card = create_card_from_input(vec);
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
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return ERROR;
    }
}

void client_interface_lib::InterfaceThread::interface_thread_main(
    std::shared_ptr<Player> player_sp,
    std::shared_ptr<std::binary_semaphore> TCP_sem,
    std::shared_ptr<std::binary_semaphore> sem_print,
    std::shared_ptr<std::binary_semaphore> interface_sem,
    int parent_pipe_read_fd,
    int parent_pipe_write_fd)
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
                btwn_thread_comm::read_msg(parent_pipe_read_fd, parent_msg);

                if (parent_msg == "TRICK")
                {
                    TCP_sem->release();
                    bool card_ok_to_be_played = false;

                    while (!card_ok_to_be_played)
                    {
                        std::getline(std::cin, input);
                        if (check_card_correctness(input, player_sp) != SUCCESS)
                        {
                            sem_print->acquire();
                            std::cerr << "ERROR: Chosen card is not correct. Try again.\n";
                            sem_print->release();
                        }
                        else
                        {
                            btwn_thread_comm::send_msg(parent_pipe_write_fd, "DONE");
                            card_ok_to_be_played = true;
                            TCP_sem->release();
                            interface_sem->acquire();
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
                    sem_print->acquire();
                    player_sp->print_available_cards();
                    sem_print->release();
                }
                else if (input == "tricks")
                {
                    sem_print->acquire();
                    player_sp->print_taken_lewas();
                    sem_print->release();
                }
                else if (input == "exit")
                {
                    btwn_thread_comm::send_msg(parent_pipe_write_fd, "EXIT");
                    return;
                }
                else if (input == "help")
                {
                    sem_print->acquire();
                    std::cout << "Available commands:\n"
                                 "cards - show available cards on your hand\n"
                                 "tricks - show taken lewas\n"
                                 "exit - exit the game\n";
                    fflush(stdout);
                    sem_print->release();
                }
                else
                {
                    sem_print->acquire();
                    std::cerr << "Unknown command. Type 'help' for available commands.\n";
                    sem_print->release();
                }
            }
        }
    }
}