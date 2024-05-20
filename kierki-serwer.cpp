#include <boost/program_options.hpp>

namespace po = boost::program_options;

#include <iostream>

int handle_programme_parameters(int ac, char *av[], po::variables_map &vm)
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("p,p", po::value<std::vector<int>>(), "<port nbr> on which server listens")
        ("f,f", po::value<std::vector<std::string>>(), "<file name> to read from")
        ("t,t", po::value<std::vector<unsigned>>(), "<timeout> in seconds");

    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << "\n";
        return 1;
    }
    if (vm.count("f") == 0)
    {
        throw std::invalid_argument("The 'f' option is required but missing.");
    }
    return 0;
}

void assign_parameters(po::variables_map &vm, int &port, unsigned &timeout, std::string &file_name)
{
    if (vm.count("p"))
    {
        auto ports = vm["p"].as<std::vector<int>>();
        // port = vm["p"].as<std::vector<int>>();
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
    // file_name = vm["f"].as<std::string>();
}

void print_parameters(int port, unsigned timeout, std::string file_name)
{
    std::cout << "port: " << port << "\n";
    std::cout << "timeout: " << timeout << "\n";
    std::cout << "file name: " << file_name << "\n";
}

int main(int ac, char *av[])
{
    try
    {
        po::variables_map vm;
        int ret = handle_programme_parameters(ac, av, vm);

        if (ret != 0)
            return ret;

        int port;
        unsigned timeout;
        std::string file_name;

        assign_parameters(vm, port, timeout, file_name); 
        print_parameters(port, timeout, file_name);
        
    }
    catch (std::invalid_argument &e)
    {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
