#ifndef BTWN_THREAD_COMM_H
#define BTWN_THREAD_COMM_H

#include <string>

namespace btwn_thread_comm
{
    void send_msg(int fd, std::string msg);

    void read_msg(int fd, std::string &msg);
}

#endif // BTWN_THREAD_COMM_H