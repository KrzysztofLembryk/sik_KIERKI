#ifndef KLIENT_AUTO_LIB_H
#define KLIENT_AUTO_LIB_H

#include "enum_types.h"
#include <sys/socket.h>
#include "address_wrapper_cls.h"

namespace klient_auto_func
{
    int klient_auto_main(AddressWrapper &server_address, 
    AddressWrapper &client_address, int socket_fd, 
    PlayerPosition chosen_position);
}

#endif 