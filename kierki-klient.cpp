#include <boost/program_options.hpp>
#include "common.h"
#include <netinet/in.h>
#include "exception_wrappers.h"
#include "init_comm_wrappers.h"
#include "TCP_handler.h"
#include <iostream>
// namespace po = boost::program_options;

int main(int argc, char *argv[])
{
    if (argc != 3) 
        printf("usage: %s <protocol type> <host> <port>\n", argv[0]);

    
    const char *host = argv[1]; 
    uint16_t port = port_from_str_to_ul(argv[2]); 
    struct sockaddr_in server_address = get_server_address(host, port); 

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (connect(socket_fd, (struct sockaddr *) &server_address,
            (socklen_t) sizeof(server_address)) < 0) 
    {
        exception_wrappers::runtime_err_wrapper(" cannot connect to the server");
    }

    init_comm_wrappers::IAM_Wrapper iam;
    iam.write(socket_fd, PlayerPosition::N);
    
    std::string packet_name;
    if (tcp::TCP_read_packet_name(socket_fd, INIT_CONN_PACKET_NAME_SIZE, packet_name) != SUCCESS)
    {
        exception_wrappers::runtime_err_wrapper("Got Wrong packet name SIZE from server");
    }
    if (packet_name == "BUSY")
    {
        std::cout << "Got BUSY packet\n";
        init_comm_wrappers::BUSY_Wrapper busy;
        std::vector<PlayerPosition> taken_positions;
        busy.read(socket_fd, taken_positions);

        for (auto pos : taken_positions)
        {
            std::cout << "Position " << (unsigned)pos << " is taken\n";
        }
    }
    else if (packet_name == "DEAL")
    {
        std::cout << "Got DEAL packet\n";
        init_comm_wrappers::DEAL_Wrapper deal;
        GameType game_type;
        PlayerPosition first_player_pos;
        cardCls::DeckOfCards deck_of_cards;
        deal.read(socket_fd, game_type, first_player_pos, deck_of_cards);
    }
    else
    {
        exception_wrappers::runtime_err_wrapper("Got Wrong packet name from server");
    }

    // char buff[] = {'I', 'A', 'M', 'K', '\r', '\n'};
    // std::cout << "Sending wrong msg\n";
    // writen(socket_fd, buff, sizeof(buff));
}