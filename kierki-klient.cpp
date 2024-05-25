#include <boost/program_options.hpp>
#include "common.h"
#include <netinet/in.h>
#include "exception_wrappers.h"
#include "communication_wrappers.h"
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

    communication_wrappers::IAM_Wrapper iam;
    char buff[] = {'I', 'A', 'M', '8', '\r', '\n'};
    std::cout << "Sending wrong msg\n";
    writen(socket_fd, buff, sizeof(buff));
}