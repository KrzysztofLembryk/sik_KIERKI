#include "address_wrapper_cls.h"
#include "constants.h"
#include "exception_wrappers.h"

void AddressWrapper::set_address(struct sockaddr_in &address)
{
    this->type_of_ip = IP4_OPT;
    this->ip4_address = address;
}

void AddressWrapper::set_address(struct sockaddr_in6 &address)
{
    this->type_of_ip = IP6_OPT;
    this->ip6_address = address;
}

struct sockaddr* AddressWrapper::get_address()
{
    if (this->type_of_ip == IP4_OPT)
    {
        return (struct sockaddr*)&ip4_address;
    }
    else if (this->type_of_ip == IP6_OPT)
    {
        return (struct sockaddr*)&ip6_address;
    }
    else
    {
        exception_wrappers::invalid_arg_wrapper("type of ip is not set");
    }
}

socklen_t AddressWrapper::get_address_len()
{
    if (this->type_of_ip == IP4_OPT)
    {
        return (socklen_t)sizeof(ip4_address);
    }
    else if (this->type_of_ip == IP6_OPT)
    {
        return (socklen_t)sizeof(ip6_address);
    }
    else
    {
        exception_wrappers::invalid_arg_wrapper("type of ip is not set");
    }
}