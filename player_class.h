#ifndef PLAYER_CLASS_H
#define PLAYER_CLASS_H

#include "game_classes.h"

enum PlayerPosition
{
    N = 0,
    E,
    S,
    W
};

class Player
{
    Player() = delete;
    Player(PlayerPosition position, GameType game_type) : position(position), points(0), point_counter(game_type) {}
    ~Player() = default;

    

private:
    deck::DeckOfCards hand;
    PlayerPosition position;
    uint8_t points;
    game::PointCounter point_counter;
    std::vector<deck::Lewa> lewas_taken;
};

#endif // PLAYER_CLASS_H