#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <netdb.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "err.h"
#include "common.h"
#include "exception_wrappers.h"
#include "constants.h"

uint16_t port_from_str_to_ul(char const *string)
{
    char *endptr;
    errno = 0;
    unsigned long port = strtoul(string, &endptr, 10);
    if (errno != 0 || *endptr != 0 || port > UINT16_MAX)
    {
        err_func::fatal("%s is not a valid port number", string);
    }
    return (uint16_t)port;
}

struct sockaddr_in get_server_address_ip4(char const *host, uint16_t port)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    struct addrinfo *address_result;
    int errcode = getaddrinfo(host, NULL, &hints, &address_result);
    if (errcode != 0)
    {
        err_func::fatal("getaddrinfo: %s", gai_strerror(errcode));
    }

    struct sockaddr_in send_address;
    send_address.sin_family = AF_INET; // IPv4
    send_address.sin_addr.s_addr =     // IP address
        ((struct sockaddr_in *)(address_result->ai_addr))->sin_addr.s_addr;
    send_address.sin_port = htons(port); // port from the command line

    freeaddrinfo(address_result);

    return send_address;
}

struct sockaddr get_server_address(char const *host, uint16_t port, 
int &type_of_ip)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    if (type_of_ip == IP6_OPT)
    {
        hints.ai_family = AF_INET6; // IPv6
    }
    else if (type_of_ip == IP4_OPT)
    {
        hints.ai_family = AF_INET; // IPv4
    }
    else
    {
        hints.ai_family = AF_UNSPEC; // IPv6
    }
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    struct addrinfo *address_result;
    int errcode = getaddrinfo(host, NULL, &hints, &address_result);
    if (errcode != 0)
    {
        err_func::fatal("getaddrinfo: %s", gai_strerror(errcode));
    }
    // struct sockaddr_in6 send_address;
    // send_address.sin6_family = AF_INET6; // IPv6
    // send_address.sin6_addr =     // IP address
    //     ((struct sockaddr_in6 *)(address_result->ai_addr))->sin6_addr;
    // send_address.sin6_port = htons(port); // port from the command line
    struct sockaddr final_addr;
    if (address_result->ai_family == AF_INET)
    {
        // IPv4
        struct sockaddr_in send_address_ip4;
        send_address_ip4.sin_family = AF_INET;
        send_address_ip4.sin_addr = ((struct sockaddr_in *)(address_result->ai_addr))->sin_addr;
        send_address_ip4.sin_port = htons(port);
        final_addr = *((struct sockaddr *)&send_address_ip4);
        type_of_ip = IP4_OPT;
        // Use send_address
    }
    else if (address_result->ai_family == AF_INET6)
    {
        // IPv6
        struct sockaddr_in6 send_address_ip6;
        send_address_ip6.sin6_family = AF_INET6;
        send_address_ip6.sin6_addr = ((struct sockaddr_in6 *)(address_result->ai_addr))->sin6_addr;
        send_address_ip6.sin6_port = htons(port);
        final_addr = *((struct sockaddr *)&send_address_ip6);
        type_of_ip = IP6_OPT;
        // Use send_address
    }
    else
    {
        // Unexpected family
        exception_wrappers::invalid_arg_wrapper("Unexpected address family");
    }

    freeaddrinfo(address_result);

    return final_addr;
}
// Following two functions come from Stevens' "UNIX Network Programming" book.
// Read n bytes from a descriptor. Use in place of read() when fd is a stream socket.
ssize_t readn(int fd, void *vptr, size_t n)
{
    ssize_t nleft, nread;
    char *ptr;

    ptr = (char *)vptr;
    nleft = n;
    while (nleft > 0)
    {
        if ((nread = read(fd, ptr, nleft)) < 0)
            return nread; // When error, return < 0.
        else if (nread == 0)
            break; // EOF

        nleft -= nread;
        ptr += nread;
    }
    return n - nleft; // return >= 0
}

// Write n bytes to a descriptor.
ssize_t writen(int fd, const void *vptr, size_t n)
{
    ssize_t nleft, nwritten;
    const char *ptr;

    ptr = (char *)vptr; // Can't do pointer arithmetic on void*.
    nleft = n;
    while (nleft > 0)
    {
        if ((nwritten = write(fd, ptr, nleft)) <= 0)
        {
            return nwritten; // error
        }

        nleft -= nwritten;
        ptr += nwritten;
    }
    return n;
}

void install_signal_handler(int signal, void (*handler)(int))
{
    struct sigaction action;
    sigset_t block_mask;

    sigemptyset(&block_mask);
    action.sa_handler = handler;
    action.sa_mask = block_mask;
    action.sa_flags = 0;

    if (sigaction(signal, &action, NULL) < 0)
    {
        err_func::syserr("sigaction");
    }
}

PlayerPosition char_to_playerPos(char pos)
{
    if (pos == 'N')
        return PlayerPosition::N;
    else if (pos == 'E')
        return PlayerPosition::E;
    else if (pos == 'S')
        return PlayerPosition::S;
    else if (pos == 'W')
        return PlayerPosition::W;
    else
    {
        exception_wrappers::invalid_arg_wrapper("Read not allowed position: " + std::string(1, pos));
    }
}

char playerPos_to_char(PlayerPosition pos)
{
    if (pos == PlayerPosition::N)
        return 'N';
    else if (pos == PlayerPosition::E)
        return 'E';
    else if (pos == PlayerPosition::S)
        return 'S';
    else if (pos == PlayerPosition::W)
        return 'W';
    else
    {
        exception_wrappers::invalid_arg_wrapper("Wrong position value");
    }
}

char gameType_to_char(GameType game_type)
{
    if (game_type == NO_LEWA)
        return '1';
    else if (game_type == NO_HEART)
        return '2';
    else if (game_type == NO_QUEEN)
        return '3';
    else if (game_type == NO_MISTER)
        return '4';
    else if (game_type == NO_KING_HEART)
        return '5';
    else if (game_type == NO_SEVEN_AND_LAST)
        return '6';
    else if (game_type == BANDIT)
        return '7';
    else
    {
        exception_wrappers::invalid_arg_wrapper("Wrong game type value");
    }
}

GameType char_to_gameType(char game_type)
{
    if (game_type == '1')
        return NO_LEWA;
    else if (game_type == '2')
        return NO_HEART;
    else if (game_type == '3')
        return NO_QUEEN;
    else if (game_type == '4')
        return NO_MISTER;
    else if (game_type == '5')
        return NO_KING_HEART;
    else if (game_type == '6')
        return NO_SEVEN_AND_LAST;
    else if (game_type == '7')
        return BANDIT;
    else
    {
        exception_wrappers::invalid_arg_wrapper("Wrong char game type value read from socket");
    }
}

uint8_t determine_value(std::vector<char> values)
{
    if (values.size() == 1)
    {

        if (values[0] == 'J')
        {
            return J;
        }
        else if (values[0] == 'Q')
        {
            return Q;
        }
        else if (values[0] == 'K')
        {
            return K;
        }
        else if (values[0] == 'A')
        {
            return A;
        }
        else if (values[0] >= '2' && values[0] <= '9')
        {
            return static_cast<uint8_t>(values[0] - '0');
        }
        else
        {
            exception_wrappers::invalid_arg_wrapper("Invalid card value read from socket");
        }
    }
    else if (values[0] == '1' && values[1] == '0')
    {
        return 10;
    }
    else
    {
        exception_wrappers::invalid_arg_wrapper("Invalid card value read from socket");
    }
}

Suit determine_suit(char suit)
{
    if (suit == Suit::HEARTS || suit == Suit::DIAMONDS || suit == Suit::CLUBS || suit == Suit::SPADES)
    {
        return static_cast<Suit>(suit);
    }
    else
    {
        exception_wrappers::invalid_arg_wrapper("Invalid card suit");
    }
}