#ifndef INGAME_COMM_WRAPPERS_H
#define INGAME_COMM_WRAPPERS_H

#include "card_classes.h"

namespace ingame_comm_wrappers
{


    class TRICK_Wrapper
    {
    public:
        TRICK_Wrapper() = default;
        ~TRICK_Wrapper() = default;

        void write(int socket_fd, cardCls::Lewa &lewa);

        int read(int socket_fd, cardCls::Lewa &lewa);

        private:
        std::vector<char> name{'T', 'R', 'I', 'C', 'K'};
    };

    class WRONG_Wrapper
    {
    public:
        WRONG_Wrapper() = default;
        ~WRONG_Wrapper() = default;

        void write(int socket_fd, uint8_t lewa_id);

        int read(int socket_fd, uint8_t &lewa_id);

    private:
        std::vector<char> name{'W', 'R', 'O', 'N', 'G'};
    };

    class TAKEN_Wrapper
    {
    public:
        TAKEN_Wrapper() = default;
        ~TAKEN_Wrapper() = default;

        void write(int socket_fd, const cardCls::Lewa &lewa, 
            const PlayerPosition &player_who_took_lewa);

        int read(int socket_fd, cardCls::Lewa &lewa, 
            PlayerPosition &player_who_took_lewa);
    private:
        std::vector<char> name{'T', 'A', 'K', 'E', 'N'};
    };
}
#endif // MAIN_COMM_WRAPPERS_H