#include "client_interface_lib.h"


void client_interface_lib::InterfaceThread::interface_thread_main(
    std::shared_ptr<Player> player_sp,
    std::shared_ptr<std::binary_semaphore> TCP_sem,
    int parent_pipe_read_fd,
    int parent_pipe_write_fd,
    std::shared_ptr<bool> thread_ended_sp)
{
    std::string input;


    while (true)
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
            
            break;
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