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
#include "player_class.h"
#include "klient_auto_lib.h"
#include "address_wrapper_cls.h"

int init_client(int argc,
                char *argv[],
                uint16_t &port,
                std::string &host,
                PlayerPosition &chosen_position,
                bool &a_opt,
                bool &ip6_opt,
                bool &ip4_opt,
                AddressWrapper &server_address,
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
            get_server_address(host.data(), port, final_type_of_ip, server_address);
        }
        else if (ip4_opt)
        {
            final_type_of_ip = IP4_OPT;
            get_server_address(host.data(), port, final_type_of_ip, server_address);
        }
        else
        {
            final_type_of_ip = NO_IP_OPT;
            get_server_address(host.data(), port, final_type_of_ip, server_address);
        }
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << "\n";
        return ERROR;
    }
    return SUCCESS;
}

void init_client_address(int socket_fd, AddressWrapper &client_address, int final_type_of_ip)
{
    socklen_t len;

    if (final_type_of_ip == IP4_OPT)
    {
        struct sockaddr_in temp_client_addr;
        len = sizeof(struct sockaddr_in);

        if (getsockname(socket_fd, (struct sockaddr *)&temp_client_addr, 
        &len) == -1)
        {
            exception_wrappers::runtime_err_wrapper("Cannot get my address ipv4");
        }
        client_address.set_address(temp_client_addr);
    }
    else
    {
        struct sockaddr_in6 temp_client_addr;
        len = sizeof(struct sockaddr_in6);

        if (getsockname(socket_fd, (struct sockaddr *)&temp_client_addr,
                        &len) == -1)
        {
            exception_wrappers::runtime_err_wrapper("Cannot get my address ipv6");
        }
        client_address.set_address(temp_client_addr);
    }
}


int main(int argc, char *argv[])
{
    try
    {
        uint16_t port;
        std::string host;
        bool a_option = false;
        bool ip6_opt = false;
        bool ip4_opt = false;
        PlayerPosition chosen_position;
        AddressWrapper server_address;
        int socket_fd;
        int final_type_of_ip;

        // Access the options
        if (init_client(argc, argv, port, host, chosen_position, a_option, ip6_opt, ip4_opt, server_address, final_type_of_ip) != SUCCESS)
        {
            return FAILURE;
        }

        socket_func::handle_client_socket_init(socket_fd, final_type_of_ip);

        AddressWrapper client_address;

        init_client_address(socket_fd, client_address, final_type_of_ip);


        if (connect(socket_fd, server_address.get_address(), 
                    server_address.get_address_len()) < 0)
        {
            err_func::error("Cannot connect to the server");
            return FAILURE;
        }

        std::string msg_str;
        init_comm_wrappers::IAM_Wrapper iam;
        std::string address_str = communication_addresses_to_str(server_address.get_address(), client_address.get_address(), 
        true, NOT_INVOKED_BY_SERVER);

        iam.write(socket_fd, chosen_position, msg_str);
        print_log_from_write(address_str, msg_str);

        if (a_option)
        {
            klient_auto_func::klient_auto_main(server_address, client_address, socket_fd, chosen_position);
        }
        else 
        {
            klient_auto_func::klient_auto_main(server_address, client_address, socket_fd, chosen_position);
        }
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << "\n";
        return FAILURE;
    }
}
