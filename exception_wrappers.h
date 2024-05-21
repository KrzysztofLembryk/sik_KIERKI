#ifndef EXCEPTION_WRAPPERS_H
#define EXCEPTION_WRAPPERS_H

#include <stdexcept>
#include <source_location>

namespace exception_wrappers
{
    void invalid_arg_wrapper(const std::string &msg, const std::source_location &loc = std::source_location::current())
    {
        std::string file_name = loc.file_name();
        std::string line = std::to_string(loc.line());
        std::string column = std::to_string(loc.column());
        std::string func_name = loc.function_name();

        throw std::invalid_argument("file: " + file_name + "(" + line + ":" + column + ") --> " + func_name + ":\n---" + msg + "---");
    }

    void runtime_err_wrapper(const std::string &msg, const std::source_location &loc = std::source_location::current())
    {
        std::string file_name = loc.file_name();
        std::string line = std::to_string(loc.line());
        std::string column = std::to_string(loc.column());
        std::string func_name = loc.function_name();

        throw std::runtime_error("file: " + file_name + "(" + line + ":" + column + ") --> " + func_name + ":\n---" + msg + "---");
    }
} // namespace exception_wrappers

#endif // EXCEPTION_WRAPPERS_H