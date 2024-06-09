#include "parameters_handling.h"
#include <iostream>
#include "exception_wrappers.h"
#include "err.h"
#include "constants.h"
#include "common.h"

namespace po = boost::program_options;

void parse_programme_parameters_server(int ac,
                                       char *av[],
                                       po::variables_map &vm)
{
    po::options_description desc("Allowed options");

    desc.add_options()("help,h", "produce help message")("p,p", po::value<std::vector<uint16_t>>(), "<port nbr> on which server listens")("f,f", po::value<std::vector<std::string>>(), "<file name> to read from")("t,t", po::value<std::vector<unsigned>>(), "<timeout> in seconds");

    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        throw std::invalid_argument("help option");
    }
    if (vm.count("f") == 0)
    {
        exception_wrappers::invalid_arg_wrapper("The 'f' option is required but missing.");
    }
}

void parse_programme_parameters_client(int argc, char *argv[], 
    std::vector<std::string> &ports, std::vector<std::string> &hosts, 
    std::map<PlayerPosition, bool> &positions, bool &a_option, bool &ip6_opt, bool &ip4_opt)
{
    int opt;

    while ((opt = getopt(argc, argv, "p:h:aNSWE64")) != -1) {
        switch (opt) 
        {
            case 'p':
                ports.push_back(optarg);
                break;
            case 'h':
                hosts.push_back(optarg);
                break;
            case 'a':
                a_option = true;
                break;
            case 'N':
                positions[N] = true;
                break;
            case 'S':
                positions[S] = true;
                break;
            case 'W':
                positions[W] = true;
                break;
            case 'E':
                positions[E] = true;
                break;
            case '6':
                ip6_opt = true;
                break;
            case '4':
                ip4_opt = true;
                break;
            default: /* '?' */
                exception_wrappers::invalid_arg_wrapper("Usage: [-p port] [-h host] [-aNSWE64]");
        }
    }
}



void assign_programme_parameters_server(po::variables_map &vm, uint16_t &port, unsigned &timeout, std::string &file_name)
{
    if (vm.count("p"))
    {
        auto ports = vm["p"].as<std::vector<uint16_t>>();
        port = ports[0];
    }
    if (vm.count("t"))
    {
        auto timeouts = vm["t"].as<std::vector<unsigned>>();
        timeout = timeouts[0];
    }
    else
        timeout = 5;

    auto file_names = vm["f"].as<std::vector<std::string>>();
    file_name = file_names[0];
}


void assign_programme_parameters_client( 
                                uint16_t &port, 
                                std::string &host,
                                std::vector<std::string> &ports, 
                                std::vector<std::string> &hosts, 
                                std::map<PlayerPosition, bool> &positions, 
                                PlayerPosition &chosen_pos)
{
    if (ports.size()) 
    {
        port = port_from_str_to_ul(ports[0].data());
    }
    else 
    {
        exception_wrappers::invalid_arg_wrapper("Port not specified");
    }

    if (hosts.size()) 
    {
        host = hosts[0];
    }
    else
    {
        exception_wrappers::invalid_arg_wrapper("Host not specified");
    }

    if (positions.size() == 0)
    {
        exception_wrappers::invalid_arg_wrapper("No position specified");
    }
    else
    {
        for (auto &pos : positions)
        {
            chosen_pos = pos.first;
            break;
        }
    }
}