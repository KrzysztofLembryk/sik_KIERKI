#ifndef MIM_ERR_H
#define MIM_ERR_H

#include <source_location>
#include <string>
#include <stdnoreturn.h>

namespace err_func
{
    // Print information about a system error and quits.
    [[noreturn]] void syserr(const char* fmt, ...);

    // Print information about an error and quits.
    [[noreturn]] void fatal(const char* fmt, ...);

    // Print information about an error and return.
    void error(const std::string &msg, 
        const std::source_location &loc = std::source_location::current());
}

#endif
