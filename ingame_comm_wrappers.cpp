#include "ingame_comm_wrappers.h"
#include "common.h"
#include "TCP_handler.h"

std::vector<char> end_chars{'\r', '\n'};

// TRICK_Wrapper impl
void ingame_comm_wrappers::TRICK_Wrapper::write(int socket_fd, 
    cardCls::Lewa  &lewa)
{
    std::vector<char> msg_vec(name);
    msg_vec.push_back(static_cast<char>(lewa.get_lewa_id()));

    if (lewa.size() == 0 )
    {
        msg_vec.insert(msg_vec.end(), end_chars.begin(), end_chars.end());
        tcp::TCP_send_packet(socket_fd, msg_vec.data(), msg_vec.size());
    }

}


