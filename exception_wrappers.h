#ifndef EXCEPTION_WRAPPERS_H
#define EXCEPTION_WRAPPERS_H

#include <source_location>
#include <string>

namespace exception_wrappers
{
    void invalid_arg_wrapper(const std::string &msg, 
    const std::source_location &loc = std::source_location::current());

    void runtime_err_wrapper(const std::string &msg, const std::source_location &loc = std::source_location::current());
} // namespace exception_wrappers

#endif // EXCEPTION_WRAPPERS_H