#ifndef POLL_FUNC_H
#define POLL_FUNC_H

#include <poll.h>
#include <string>

namespace polls_func
{
    int handle_polls_waiting(int &poll_status, struct pollfd *poll_descriptors);

    void handle_polls_read(int pipe_fd, std::string &msg);

} // namespace polls_func


#endif // POLL_FUNC_H