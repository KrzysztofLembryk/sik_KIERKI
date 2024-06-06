#include <boost/program_options.hpp>
#include "common.h"
#include <netinet/in.h>
#include "exception_wrappers.h"
#include "init_comm_wrappers.h"
#include "ingame_comm_wrappers.h"
#include "TCP_handler.h"
#include <iostream>
#include "err.h"
#include "constants.h"

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("p,p", po::value<std::vector<uint16_t>>()->multitoken(), "set port")
        ("h,h", po::value<std::vector<std::string>>()->multitoken(), "set host")
        ("a", po::value<std::vector<int>>()->zero_tokens()->multitoken(), "auto player")
        ("N", po::value<std::vector<int>>()->zero_tokens()->multitoken(), "N")
        ("S", po::value<std::vector<int>>()->zero_tokens()->multitoken(), "S")
        ("W", po::value<std::vector<int>>()->zero_tokens()->multitoken(), "W")
        ("E", po::value<std::vector<int>>()->zero_tokens()->multitoken(), "E")
        ("6", po::value<std::vector<int>>()->zero_tokens()->multitoken(), "IP6")
        ("4", po::value<std::vector<int>>()->zero_tokens()->multitoken(), "IP4");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }

    uint16_t port = 0;

    // Access the options
    if (vm.count("port")) {
        auto ports = vm["port"].as<std::vector<uint16_t>>();
        port = ports[0];
        // Use the ports...
    }
    else 
    {
        err_func::error("Port not specified");    
        return ERROR;
    }

    if (vm.count("h")) {
        std::vector<std::string> hosts = vm["host"].as<std::vector<std::string>>();
        // Use the hosts...
    }
    else
    {
        err_func::error("Host not specified");
        return ERROR;
    }

    // Check for options without values
    if (vm.count("a")) {
        // Option -a was specified
    }
}



// int main(int argc, char *argv[])
// {
//     if (argc != 4) 
//         exception_wrappers::invalid_arg_wrapper("usage: <host> <port> <position>");

    
//     const char *host = argv[1]; 
//     uint16_t port = port_from_str_to_ul(argv[2]); 
//     struct sockaddr_in server_address = get_server_address(host, port); 

//     int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

//     if (connect(socket_fd, (struct sockaddr *) &server_address,
//             (socklen_t) sizeof(server_address)) < 0) 
//     {
//         exception_wrappers::runtime_err_wrapper(" cannot connect to the server");
//     }

//     init_comm_wrappers::IAM_Wrapper iam;
//     PlayerPosition pos = char_to_playerPos(argv[3][0]); 
//     iam.write(socket_fd, pos);
    
//     std::string packet_name;
//     if (tcp::TCP_read_packet_name(socket_fd, INIT_PACKET_NAME_SIZE, packet_name) != SUCCESS)
//     {
//         exception_wrappers::runtime_err_wrapper("Got Wrong packet name SIZE from server");
//     }
//     if (packet_name == "BUSY")
//     {
//         std::cout << "Got BUSY packet\n";
//         init_comm_wrappers::BUSY_Wrapper busy;
//         std::vector<PlayerPosition> taken_positions;
//         busy.read(socket_fd, taken_positions);

//         for (auto pos : taken_positions)
//         {
//             std::cout << "Position " << (unsigned)pos << " is taken\n";
//         }
//     }
//     else if (packet_name == "DEAL")
//     {
//         std::cout << "Got DEAL packet\n";
//         init_comm_wrappers::DEAL_Wrapper deal;
//         GameType game_type;
//         PlayerPosition first_player_pos;
//         cardCls::DeckOfCards deck_of_cards;
//         deal.read(socket_fd, game_type, first_player_pos, deck_of_cards);
//         std::cout << "first_player_pos " << (unsigned)first_player_pos << "\n";
//         deck_of_cards.print_deck();
//     }
//     else
//     {
//         exception_wrappers::runtime_err_wrapper("Got Wrong packet name from server");
//     }

//     std::cout << "Waiting for Ingame packets\n";
//     if (tcp::TCP_read_packet_name(socket_fd, INGAME_PACKET_NAME_SIZE, packet_name) != SUCCESS)
//     {
//         exception_wrappers::runtime_err_wrapper("Got Wrong packet name SIZE from server");
//     }

//     if (packet_name == "TRICK")
//     {
//         std::cout << "Got TRICK packet\n";
//         ingame_comm_wrappers::TRICK_Wrapper trick;
//         cardCls::Lewa lewa;
//         uint8_t curr_round = 1;
//         trick.read(socket_fd, lewa, curr_round);
//         lewa.print();
//     }
//     else if (packet_name == "TOTAL")
//     {
//         std::cout << "Got TOTAL packet\n";
//         ingame_comm_wrappers::TOTAL_Wrapper total;
//         std::map<PlayerPosition, uint32_t> total_scores;
//         total.read(socket_fd, total_scores);
//         for (auto elem : total_scores)
//         {
//             std::cout << "Position: " << (unsigned)elem.first << " score: " << elem.second << "\n";
//         }
//     }
//     else if (packet_name == "SCORE")
//     {
//         std::cout << "Got SCORE packet\n";
//         ingame_comm_wrappers::SCORE_Wrapper score;
//         std::map<PlayerPosition, uint8_t> scores;
//         score.read(socket_fd, scores);
//         for (auto elem : scores)
//         {
//             std::cout << "Position: " << (unsigned)elem.first << " score: " << (unsigned)elem.second << "\n";
//         }
//     }
//     else if (packet_name == "WRONG")
//     {
//         std::cout << "Got WRONG packet\n";
//         ingame_comm_wrappers::WRONG_Wrapper wrong;
//         cardCls::Lewa lewa;
//         wrong.read(socket_fd, lewa, 1);
//         lewa.print();
//     }
//     else if (packet_name == "TAKEN")
//     {
//         std::cout << "Got TAKEN packet\n";
//         ingame_comm_wrappers::TAKEN_Wrapper taken;
//         PlayerPosition taken_pos;
//         cardCls::Lewa lewa;
//         taken.read(socket_fd, lewa, taken_pos, 1);
//         lewa.print();
//         std::cout << "player who took the trick: " << (unsigned)taken_pos << "\n";
//     }
//     else
//     {
//         exception_wrappers::runtime_err_wrapper("Got Wrong packet name from server");
//     }



//     // char buff[] = {'I', 'A', 'M', 'K', '\r', '\n'};
//     // std::cout << "Sending wrong msg\n";
//     // writen(socket_fd, buff, sizeof(buff));
// }