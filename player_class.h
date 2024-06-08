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
            struct sockaddr *server_addr) : 
                                                hand(hand), 
                                                position(position), 
                                                all_points(0),
                                                curr_game_points(0),
                                                point_counter(game_type), 
                                                server_address(server_addr),
                                                was_my_addr_allocated(false) {}
    ~Player()
    {
        // We have deallocate memory for client_address since we allocated it
        // in game master add_new player method
        struct sockaddr_in6 *server_addr_6 = (struct sockaddr_in6*)my_address;
        delete server_addr_6;
    }

    void set_hand(cardCls::DeckOfCards &hand);
    void set_player_address(struct sockaddr *addr);
    void set_card_played(cardCls::CardClassWrapper &card);
    void set_game_type(GameType game_type);

    cardCls::DeckOfCards get_hand();
    PlayerPosition get_position();
    uint8_t get_curr_score();
    uint32_t get_all_points();
    struct sockaddr* get_server_address();
    struct sockaddr* get_client_address();

    int check_card_correctness(cardCls::CardClassWrapper &card, 
    Suit bottom_card_suit);
    void add_points_in_curr_round(cardCls::Lewa &lewa);
    void add_points_from_round_to_allpoints();
    void add_lewa_to_lewas_taken(cardCls::Lewa &lewa);
    void clea_lewas_taken();

    cardCls::CardClassWrapper play_card(Suit bottom_card_suit);

private:
    cardCls::DeckOfCards hand;
    PlayerPosition position;
    uint32_t all_points;
    uint8_t curr_game_points;
    gameCls::PointCounter point_counter;
    std::vector<cardCls::Lewa> lewas_taken;
    // we dont need to allocate memory for server address since it lives in main
    // thread and main thread ends after all threads end, thus it is safe to 
    // use it
    struct sockaddr *server_address;
    struct sockaddr *my_address;
    bool was_my_addr_allocated;
};

#endif // PLAYER_CLASS_H