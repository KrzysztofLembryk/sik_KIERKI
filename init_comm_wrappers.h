#ifndef INIT_COMM_WRAPPERS_H
#define INIT_COMM_WRAPPERS_H

#include <vector>
#include "card_classes.h"
#include "constants.h"

/**
 * IDEA OF READING FROM SOCKETS! --> Client after sending IAM will read only 
 * first 4 bytes of packet so that we can check type of packet, because after 
 * IAM we can get either BUSY or DEAL - we can assume server is trustworthy.
 * Then we will use *_Wrapper::read to read rest of the packet, check if correct
 * etc.
*/


namespace init_comm_wrappers
{
    class IAM_Wrapper
    {
    public:
        IAM_Wrapper() = default;
        ~IAM_Wrapper() = default;

        void write(int socket_fd, PlayerPosition position);
        int read(int socket_fd, PlayerPosition &position);

    private:
        // typedef struct __attribute__((__packed__)) IAM
        // {
        //     char name[3]{'I', 'A', 'M'};
        //     char position;
        //     char end[2]{'\r', '\n'};
        // } IAM;

        // IAM iam;
        const std::vector<char> name{'I', 'A', 'M'};
    };

    class BUSY_Wrapper
    {
    public:
        BUSY_Wrapper() = default;
        ~BUSY_Wrapper() = default;

        void write(int socket_fd, std::vector<PlayerPosition> taken_positions);
        
        int read(int socket_fd, std::vector<PlayerPosition> &taken_positions);

    private:
        const std::vector<char> name{'B', 'U', 'S', 'Y'};

    };

    class DEAL_Wrapper
    {
    public:
        DEAL_Wrapper() = default;
        ~DEAL_Wrapper() = default;

        void write(int socket_fd, GameType game_type , PlayerPosition first_player_pos, cardCls::DeckOfCards &&deck_of_cards);

        int read(int socket_fd, GameType &game_type, PlayerPosition &first_player_pos, cardCls::DeckOfCards &deck_of_cards);

    private:
        std::vector<char> name{'D', 'E', 'A', 'L'};
        std::vector<char> game_type_and_first_player_pos;
    };

}


#endif