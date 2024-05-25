#ifndef PARAMETERS_HANDLING_H
#define PARAMETERS_HANDLING_H

#include <boost/program_options.hpp>
#include <string>

void parse_programme_parameters_server(int ac, char *av[],
                                       boost::program_options::variables_map &vm);


void parse_programme_parameters_client(int ac, char *av[],
                                       boost::program_options::variables_map &vm);

void assign_programme_parameters_server(
    boost::program_options::variables_map &vm,
    uint16_t &port,
    unsigned &timeout,
    std::string &file_name);

void print_parameters(uint16_t port, unsigned timeout, std::string file_name);

#endif // PARAMETERS_HANDLING_H