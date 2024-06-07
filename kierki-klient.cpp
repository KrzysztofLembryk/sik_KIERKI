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
#include "socket_fd_handler.h"


int init_client(int argc,
                char *argv[],
                uint16_t &port,
                std::string &host,
                PlayerPosition &chosen_position,
                bool &a_opt,
                bool &ip6_opt,
                bool &ip4_opt,
                struct sockaddr &server_address,
                int &final_type_of_ip)
{
    try
    {
        std::vector<std::string> ports;
        std::vector<std::string> hosts;
        std::map<PlayerPosition, bool> positions;
        parse_programme_parameters_client(argc, argv, ports, hosts, positions, a_opt, ip6_opt, ip4_opt);

        assign_programme_parameters_client(port, host, ports, hosts, positions, a_opt, ip6_opt, ip4_opt, chosen_position);

        if (ip6_opt)
        {
            final_type_of_ip = IP6_OPT;
            server_address = get_server_address(host.data(), port, final_type_of_ip);
        }
        else if (ip4_opt)
        {
            final_type_of_ip = IP4_OPT;
            server_address = get_server_address(host.data(), port, final_type_of_ip);
        }
        else
        {
            final_type_of_ip = NO_IP_OPT;
            server_address = get_server_address(host.data(), port, final_type_of_ip);
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
    PlayerPosition chosen_position;
    struct sockaddr server_address;
    int socket_fd;
    int final_type_of_ip;

    // Access the options
    if (init_client(argc, argv, port, host, chosen_position, a_option, ip6_opt, ip4_opt, server_address, final_type_of_ip) != SUCCESS)
    {
        return FAILURE;
    }

    socket_func::handle_client_socket_init(socket_fd, final_type_of_ip);

    struct sockaddr client_address;
    socklen_t len;
    
    if (final_type_of_ip == IP4_OPT)
        len = sizeof(struct sockaddr_in);
    else 
        len = sizeof(struct sockaddr_in6);

    if (getsockname(socket_fd, (struct sockaddr *)&client_address, &len) == -1)
    {
        err_func::error("Cannot get my address");
        return FAILURE;
    }

    if (connect(socket_fd, &server_address, (socklen_t)sizeof(server_address)) < 0)
    {
        err_func::error("Cannot connect to the server");
        return FAILURE;
    }

}
