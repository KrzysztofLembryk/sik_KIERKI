#ifndef MIM_COMMON_H
#define MIM_COMMON_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include "enum_types.h"
#include <vector>

uint16_t port_from_str_to_ul(char const *string);

// struct sockaddr_in get_server_address_ip4(char const *host, uint16_t port);
struct sockaddr get_server_address(char const *host, uint16_t port, int &type_of_ip);

ssize_t	readn(int fd, void *vptr, size_t n);
ssize_t	writen(int fd, const void *vptr, size_t n);
void install_signal_handler(int signal, void (*handler)(int));


PlayerPosition char_to_playerPos(char pos);

char playerPos_to_char(PlayerPosition pos);

char gameType_to_char(GameType game_type);

GameType char_to_gameType(char game_type);

uint8_t determine_value(std::vector<char> values);

Suit determine_suit(char suit);

void print_communication_addresses(const struct sockaddr &server_address, const struct sockaddr &client_address, bool client_sent_msg);

#endif
