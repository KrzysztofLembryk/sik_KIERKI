#ifndef PLAYER_CLASS_H
#define PLAYER_CLASS_H

#include "game_classes.h"


class Player
{
    Player() = default;
    // Player(PlayerPosition position, GameType game_type) : position(position), points(0), point_counter(game_type) {}
    ~Player() = default;

    

private:
    cardCls::DeckOfCards hand;
    PlayerPosition position;
    uint64_t points;
    gameCls::PointCounter point_counter;
    std::vector<cardCls::Lewa> lewas_taken;
    // -!! Informacja o adresie i porcie klienta, kiedy klient sie rozlaczy !!-
    // i przyjdzie jakis nowy to bedziemy zmieniac adres i port
};

#endif // PLAYER_CLASS_H