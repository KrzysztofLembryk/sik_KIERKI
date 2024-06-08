#ifndef ADDRESS_WRAPPER_CLS_H
#define ADDRESS_WRAPPER_CLS_H

#include <netinet/in.h>


class AddressWrapper
{
    public:
    AddressWrapper() : type_of_ip(0) {};

    void set_address(struct sockaddr_in &address);
    void set_address(struct sockaddr_in6 &address);
    struct sockaddr* get_address();
    socklen_t get_address_len();
    
private:
    struct sockaddr_in6 ip6_address;
    struct sockaddr_in ip4_address;
    int type_of_ip;
};


#endif // ADDRESS_WRAPPER_CLS_H