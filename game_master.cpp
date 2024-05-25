#include "game_master.h"

gm::GameMaster::GameMaster(std::vector<gameCls::Round> &rounds, const struct sockaddr_in6 &server_addr) : 
curr_lewa(0), 
seat_taken_map({{N, false}, {E, false}, {S, false}, {W, false}})
{
    this->rounds = rounds;
    this->round_number = 0;
    this->whose_turn = rounds[0].get_first_player();
    this->card_counter.new_game(rounds[0].get_game_type());

    std::vector<PlayerPosition> player_pos({N, E, S, W});

    for (auto pos : player_pos)
    {
        this->players[pos] = std::make_shared<Player>(rounds[0].get_player_cards(pos), pos, rounds[0].get_game_type(), server_addr);
    }
}

bool gm::GameMaster::check_if_seat_taken(PlayerPosition pos)
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    return seat_taken_map[pos];
}


