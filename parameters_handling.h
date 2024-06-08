#ifndef PARAMETERS_HANDLING_H
#define PARAMETERS_HANDLING_H

#include <boost/program_options.hpp>
#include <string>
#include "enum_types.h"

void parse_programme_parameters_server(int ac, char *av[],
                                       boost::program_options::variables_map &vm);

void parse_programme_parameters_client(int argc, 
                                    char *argv[], 
                                    std::vector<std::string> &ports, 
                                    std::vector<std::string> &hosts, 
                                    std::map<PlayerPosition, bool> &positions, 
                                    bool &a_option, 
                                    bool &ip6_opt, 
                                    bool &ip4_opt);

void assign_programme_parameters_server(
    boost::program_options::variables_map &vm,
    uint16_t &port,
    unsigned &timeout,
    std::string &file_name);


void assign_programme_parameters_client( 
                                uint16_t &port, 
                                std::string &host,
                                std::vector<std::string> &ports, 
                                std::vector<std::string> &hosts, 
                                std::map<PlayerPosition, bool> &positions, 
                                bool &a_option, bool &ip6_opt, bool &ip4_opt,
                                PlayerPosition &chosen_pos);

#endif // PARAMETERS_HANDLING_H