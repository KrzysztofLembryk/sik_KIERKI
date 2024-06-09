#include "exception_wrappers.h"
#include <stdexcept>

[[noreturn]] void exception_wrappers::invalid_arg_wrapper(const std::string &msg, const std::source_location &loc) 
{
    std::string file_name = loc.file_name();
    std::string line = std::to_string(loc.line());
    std::string column = std::to_string(loc.column());
    std::string func_name = loc.function_name();

    throw std::invalid_argument("\tINVALID_ARG_EXCEPTION: " + file_name + "(" + line + ":" + column + ") --> " + func_name + ":\n\t---" + msg + "---\n");
}

 [[noreturn]] void exception_wrappers::runtime_err_wrapper(const std::string &msg, const std::source_location &loc) 
{
    std::string file_name = loc.file_name();
    std::string line = std::to_string(loc.line());
    std::string column = std::to_string(loc.column());
    std::string func_name = loc.function_name();

    throw std::runtime_error("\tRUNTIME_EXCEPTION: "+ file_name + "(" + line + ":" + column + ") --> " + func_name + ":\n\t---" + msg + "---\n");
}