#ifndef PLAYER_CLASS_H
#define PLAYER_CLASS_H

#include "game_classes.h"
#include <netinet/in.h>


class Player
{
public:
    Player() = delete;
    Player( cardCls::DeckOfCards &hand, 
            PlayerPosition position, 
            GameType game_type, 
            const struct sockaddr_in6 &server_addr) : 
                                                hand(hand), 
                                                position(position), 
                                                all_points(0),
                                                curr_game_points(0),
                                                point_counter(game_type), 
                                                server_address(server_addr) {}
    ~Player() = default;

    void set_hand(cardCls::DeckOfCards &hand);
    void set_player_address(const struct sockaddr_in6 &addr);
    void set_card_played(cardCls::CardClassWrapper &card);
    void set_game_type(GameType game_type);

    cardCls::DeckOfCards get_hand();
    PlayerPosition get_position() const;

    int check_card_correctness(cardCls::CardClassWrapper &card, 
    Suit bottom_card_suit);
    void add_points_in_curr_round(cardCls::Lewa &lewa);
    void add_points_from_round_to_allpoints();

private:
    cardCls::DeckOfCards hand;
    const PlayerPosition position;
    uint32_t all_points;
    uint8_t curr_game_points;
    gameCls::PointCounter point_counter;
    std::vector<cardCls::Lewa> lewas_taken;
    const struct sockaddr_in6 server_address;
    struct sockaddr_in6 my_address;
};

#endif // PLAYER_CLASS_H