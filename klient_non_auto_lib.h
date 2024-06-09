#ifndef KLIENT_NON_AUTO_LIB_H
#define KLIENT_NON_AUTO_LIB_H

#include "enum_types.h"
#include <sys/socket.h>
#include "address_wrapper_cls.h"

namespace klient_non_auto_func 
{
    
    int klient_non_auto_main(int socket_fd, 
    PlayerPosition chosen_position);
}


#endif // KLIENT_NON_AUTO_LIB_H