#ifndef COMMUNICATION_WRAPPERS_H
#define COMMUNICATION_WRAPPERS_H

#include <vector>
#include "card_classes.h"
#include "constants.h"

namespace communication_wrappers
{
    class IAM_Wrapper
    {
    public:
        IAM_Wrapper() = default;
        ~IAM_Wrapper() = default;

        void write(int socket_fd, PlayerPosition position);
        int read(int socket_fd);
        PlayerPosition get_position(); 

    private:
        typedef struct __attribute__((__packed__)) IAM
        {
            char name[3]{'I', 'A', 'M'};
            char position;
            char end[2]{'\r', '\n'};
        } IAM;

        IAM iam;
        char read_buff[IAM_BUFF_SIZE];
        PlayerPosition position;
    };

    class BUSY_Wrapper
    {
    public:
        BUSY_Wrapper() = default;
        ~BUSY_Wrapper() = default;

        void write(int socket_fd, std::vector<PlayerPosition> taken_positions);
        
        // For read we will need to check if first 4 bytes are 'BUSY'
        std::vector<PlayerPosition> read(int socket_fd);

    private:

        std::vector<char> name{'B', 'U', 'S', 'Y'};
        char buff[10];

    };

    class DEAL_Wrapper
    {
    public:
        DEAL_Wrapper() = default;
        ~DEAL_Wrapper() = default;

        void write(int socket_fd, GameType &game_type , PlayerPosition &first_player_pos, cardCls::DeckOfCards &deck_of_cards);

        void read(int socket_fd);

    private:
        std::vector<char> name{'D', 'E', 'A', 'L'};
        std::vector<char> game_type_and_first_player_pos;
        char buff[50];
    };

    class TRICK_Wrapper
    {
    public:
        TRICK_Wrapper() = default;
        ~TRICK_Wrapper() = default;

        void write(int socket_fd, cardCls::Lewa  &lewa);

        void read(int socket_fd);

        private:
        std::vector<char> name{'T', 'R', 'I', 'C', 'K'};
        char buff[17];
    };

    class WRONG_Wrapper
    {
    public:
        WRONG_Wrapper() = default;
        ~WRONG_Wrapper() = default;

        void write(int socket_fd, uint8_t lewa_id);

        void read(int socket_fd);

    private:
        std::vector<char> name{'W', 'R', 'O', 'N', 'G'};
        char buff[9];
    };

    class TAKEN_Wrapper
    {
    public:
        TAKEN_Wrapper() = default;
        ~TAKEN_Wrapper() = default;

        void write(int socket_fd, cardCls::Lewa &lewa, 
            PlayerPosition &player_who_took_lewa);

        void read(int socket_fd);
    private:
        std::vector<char> name{'T', 'A', 'K', 'E', 'N'};
        char buff[23];
    };
}


#endif