#ifndef POLL_FUNC_H
#define POLL_FUNC_H

#include <poll.h>

namespace polls_func
{
    void handle_polls_waiting(int &poll_status, struct pollfd *poll_descriptors);

    void handle_polls_read(char *pipe_buff, int *pipe_fd);

} // namespace polls_func


#endif // POLL_FUNC_H