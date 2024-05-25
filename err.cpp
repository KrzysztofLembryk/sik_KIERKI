#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "err.h"

[[noreturn]] void err_func::syserr(const char* fmt, ...) {
    va_list fmt_args;
    int org_errno = errno;

    fprintf(stderr, "\tERROR: ");

    va_start(fmt_args, fmt);
    vfprintf(stderr, fmt, fmt_args);
    va_end(fmt_args);

    fprintf(stderr, " (%d; %s)\n", org_errno, strerror(org_errno));
    exit(1);
}

[[noreturn]] void err_func::fatal(const char* fmt, ...) {
    va_list fmt_args;

    fprintf(stderr, "\tERROR: ");

    va_start(fmt_args, fmt);
    vfprintf(stderr, fmt, fmt_args);
    va_end(fmt_args);

    fprintf(stderr, "\n");
    exit(1);
}


void err_func::error(const std::string &msg, const std::source_location &loc) {

    std::string file_name = loc.file_name();
    std::string line = std::to_string(loc.line());
    std::string column = std::to_string(loc.column());
    std::string func_name = loc.function_name();

    std::cerr << "\tERROR: " << file_name + "(" + line + ":" + column + ") --> " + func_name + ":\n\t---" + msg + "---\n";
}