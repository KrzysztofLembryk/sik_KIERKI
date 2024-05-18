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
private:
    deck::DeckOfCards hand;
    PlayerPosition position;
    int points;
};

#endif // PLAYER_CLASS_H