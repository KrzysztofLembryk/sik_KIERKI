#include "parameters_handling.h"
#include <iostream>
#include "exception_wrappers.h"

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


void parse_programme_parameters_client(int ac,
                                       char *av[],
                                       po::variables_map &vm)
{
    po::options_description desc("Allowed options");

    desc.add_options()
    ("p,p", po::value<std::vector<uint16_t>>(), "<port nbr> on which server listens")
    ("h,h", po::value<std::vector<std::string>>(), "<host addr>")
    ("t,t", po::value<std::vector<unsigned>>(), "<timeout> in seconds");

    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        throw std::invalid_argument("help option");
    }
    if (vm.count("p") == 0 || vm.count("h") == 0)
    {
        exception_wrappers::invalid_arg_wrapper("The 'p' and 'h' options are required but missing.");
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

void print_parameters(uint16_t port, unsigned timeout, std::string file_name)
{
    std::cout << "port: " << port << "\n";
    std::cout << "timeout: " << timeout << "\n";
    std::cout << "file name: " << file_name << "\n";
}
