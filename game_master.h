#ifndef GAME_MASTER_H
#define GAME_MASTER_H

#include "player_class.h"
#include <memory>


namespace gm
{
    class GameMaster
    {
    public:
        GameMaster(std::vector<gameCls::Round> &rounds);

        private:
        std::map<PlayerPosition, std::shared_ptr<Player>> players;
        std::vector<gameCls::Round> rounds;
        cardCls::Lewa curr_lewa;
        PlayerPosition whose_turn;
        size_t round_number;
        gameCls::CardCounter card_counter;
    };
}


#endif