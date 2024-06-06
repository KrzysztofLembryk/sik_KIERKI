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
#include "parameters_handling.h"

namespace po = boost::program_options;

int init_client(int argc,
                char *argv[],
                uint16_t &port,
                std::string &host,
                std::map<PlayerPosition, bool> &positions,
                bool &a_opt,
                bool &ip6_opt,
                bool &ip4_opt,
                struct sockaddr &server_address)
{
    try
    {
        std::vector<std::string> ports;
        std::vector<std::string> hosts;
        parse_programme_parameters_client(argc, argv, ports, hosts, positions, a_opt, ip6_opt, ip4_opt);

        assign_programme_parameters_client(port, host, ports, hosts, positions, a_opt, ip6_opt, ip4_opt); 
        int type_of_ip;
        if (ip6_opt)
        {
            type_of_ip = 6;
            server_address = get_server_address(host.data(), port, type_of_ip);
        }
        else if (ip4_opt)
        {
            type_of_ip = 4;
            server_address = get_server_address(host.data(), port, type_of_ip);
        }
        else
        {
            type_of_ip = 0;
            server_address = get_server_address(host.data(), port, type_of_ip);
        }

    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << "\n"; 
        return ERROR;
    }
    return SUCCESS;
}

int main(int argc, char *argv[])
{
    uint16_t port;
    std::string host;
    bool a_option = false;
    bool ip6_opt = false;
    bool ip4_opt = false;
    std::map<PlayerPosition, bool> positions;
    struct sockaddr server_address;
    int socket_fd;

    // Access the options
    if (init_client(argc, argv, port, host, positions, a_option, ip6_opt, ip4_opt, server_address) != SUCCESS)
    {
        return ERROR;
    }

    // struct sockaddr_in6 server_address = get_server_address_ip4(host.data(), port);
}

// int main(int argc, char *argv[])
// {
//     if (argc != 4)
//         exception_wrappers::invalid_arg_wrapper("usage: <host> <port> <position>");

//     const char *host = argv[1];
//     uint16_t port = port_from_str_to_ul(argv[2]);
//     struct sockaddr_in server_address = get_server_address_ip4(host, port);

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