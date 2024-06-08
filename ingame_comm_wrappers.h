#ifndef INGAME_COMM_WRAPPERS_H
#define INGAME_COMM_WRAPPERS_H

#include "card_classes.h"
#include <string>

namespace ingame_comm_wrappers
{


    class TRICK_Wrapper
    {
    public:
        TRICK_Wrapper() = default;
        ~TRICK_Wrapper() = default;

        void write(int socket_fd, cardCls::Lewa &lewa, std::string &msg);

        int read(int socket_fd, cardCls::Lewa &lewa, uint8_t curr_lewa_id, std::string &msg);

        private:
        std::vector<char> name{'T', 'R', 'I', 'C', 'K'};
    };

    class WRONG_Wrapper
    {
    public:
        WRONG_Wrapper() = default;
        ~WRONG_Wrapper() = default;

        void write(int socket_fd, const cardCls::Lewa &lewa, std::string &msg);

        int read(int socket_fd, cardCls::Lewa &lewa, uint8_t curr_lewa_id, std::string &msg);

    private:
        std::vector<char> name{'W', 'R', 'O', 'N', 'G'};
    };

    class TAKEN_Wrapper
    {
    public:
        TAKEN_Wrapper() = default;
        ~TAKEN_Wrapper() = default;

        void write(int socket_fd, const cardCls::Lewa &lewa, 
            const PlayerPosition &player_who_took_lewa, std::string &msg);

        int read(int socket_fd, cardCls::Lewa &lewa, 
            PlayerPosition &player_who_took_lewa, uint8_t curr_lewa_id, std::string &msg);
    private:
        std::vector<char> name{'T', 'A', 'K', 'E', 'N'};
    };


class SCORE_Wrapper
{
public:
    SCORE_Wrapper() = default;
    ~SCORE_Wrapper() = default;

    void write(int socket_fd, const std::map<PlayerPosition, uint8_t> &scores, std::string &msg);

    int read(int socket_fd, std::map<PlayerPosition, uint8_t> &scores, std::string &msg);

private:
    std::vector<char> name{'S', 'C', 'O', 'R', 'E'};
};

class TOTAL_Wrapper
{
public:
    TOTAL_Wrapper() = default;
    ~TOTAL_Wrapper() = default;

    void write(int socket_fd, 
        const std::map<PlayerPosition, uint32_t> &total_scores, 
        std::string &msg);

    int read(int socket_fd, std::map<PlayerPosition, uint32_t> &total_scores,
    std::string &msg);

private:
    std::vector<char> name{'T', 'O', 'T', 'A', 'L'};
};

}
#endif // MAIN_COMM_WRAPPERS_H