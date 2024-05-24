#include "game_master.h"

gm::GameMaster::GameMaster(std::vector<gameCls::Round> &rounds) : curr_lewa(0)
{
    this->rounds = rounds;
    this->round_number = 0;
    this->whose_turn = rounds[0].get_first_player();
    this->card_counter.new_game(rounds[0].get_game_type());

    std::vector<PlayerPosition> player_pos({N, E, S, W});

    for (auto pos : player_pos)
    {
        this->players[pos] = std::make_shared<Player>(rounds[0].get_player_cards(pos), pos, rounds[0].get_game_type());
        // this->players[pos]->set_hand(rounds[0].get_player_cards(pos));
    }
}