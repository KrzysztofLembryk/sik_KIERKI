#ifndef PLAYER_CLASS_H
#define PLAYER_CLASS_H

#include "game_classes.h"
#include <netinet/in.h>


class Player
{
public:
    Player() = default;
    Player( cardCls::DeckOfCards &hand, 
            PlayerPosition position, 
            GameType game_type, 
            const struct sockaddr_in6 &server_addr) : 
                                                hand(hand), 
                                                position(position), 
                                                points(0), 
                                                point_counter(game_type), 
                                                server_address(server_addr) {}
    ~Player() = default;

    void set_hand(cardCls::DeckOfCards &hand);
    void set_player_address(const struct sockaddr_in6 &addr);

private:
    cardCls::DeckOfCards hand;
    const PlayerPosition position;
    uint64_t points;
    gameCls::PointCounter point_counter;
    std::vector<cardCls::Lewa> lewas_taken;
    const struct sockaddr_in6 server_address;
    struct sockaddr_in6 my_address;
};

#endif // PLAYER_CLASS_H