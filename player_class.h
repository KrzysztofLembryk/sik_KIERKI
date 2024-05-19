#ifndef PLAYER_CLASS_H
#define PLAYER_CLASS_H

#include "card_structs.h"

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
    Player(PlayerPosition position) : position(position), points(0) {}
    ~Player() = default;

    

private:
    deck::DeckOfCards hand;
    PlayerPosition position;
    uint8_t points;
};

#endif // PLAYER_CLASS_H