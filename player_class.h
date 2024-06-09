#ifndef PLAYER_CLASS_H
#define PLAYER_CLASS_H

#include "game_classes.h"
#include <netinet/in.h>
#include "address_wrapper_cls.h"
#include "socket_fd_handler.h"
#include <memory>

class Player
{
public:
    Player() = delete;
    Player(cardCls::DeckOfCards &hand,
           PlayerPosition position,
           GameType game_type) : hand(hand),
                                 position(position),
                                 all_points(0),
                                 curr_game_points(0),
                                 point_counter(game_type),
                                 curr_lewa_bottom_suit(Suit::NONE_SUIT)
    {
        client_fd_sp = std::make_shared<ClientFdWrapper>();
    }
    ~Player() = default;

    void set_hand(cardCls::DeckOfCards &hand);

    void set_player_address(struct sockaddr_in &addr);
    void set_player_address(struct sockaddr_in6 &addr);
    void set_player_address(AddressWrapper &addr);
    void set_server_address(struct sockaddr_in &addr);
    void set_server_address(struct sockaddr_in6 &addr);
    void set_server_address(AddressWrapper &addr);

    void set_client_fd(std::shared_ptr<ClientFdWrapper> client_fd_sp);
    void set_card_played(cardCls::CardClassWrapper &card);
    void set_game_type(GameType game_type);
    void set_card_played(cardCls::Lewa &lewa);
    void set_curr_lewa_bottom_suit(Suit bottom_suit);
    void set_chosen_card_by_human_player(cardCls::CardClassWrapper &card);

    cardCls::DeckOfCards get_hand();
    PlayerPosition get_position();
    uint8_t get_curr_score();
    uint32_t get_all_points();
    struct sockaddr *get_server_address();
    struct sockaddr *get_client_address();
    int get_client_fd();
    Suit get_curr_lewa_bottom_suit();
    cardCls::CardClassWrapper get_chosen_card_by_human_player();

    int check_card_correctness(cardCls::CardClassWrapper &card,
                               Suit bottom_card_suit);
    void add_points_in_curr_round(cardCls::Lewa &lewa);
    void add_points_from_round_to_allpoints();
    void add_lewa_to_lewas_taken(cardCls::Lewa &lewa);
    void clear_lewas_taken();
    void zero_curr_SCORE();
    void reset_client_fd();

    cardCls::CardClassWrapper play_card(Suit bottom_card_suit);
    void print_available_cards();
    void print_taken_lewas();

private:
    cardCls::DeckOfCards hand;
    PlayerPosition position;
    uint32_t all_points;
    uint8_t curr_game_points;
    gameCls::PointCounter point_counter;
    std::vector<cardCls::Lewa> lewas_taken;
    Suit curr_lewa_bottom_suit;
    cardCls::CardClassWrapper chosen_card_by_human_player;
    // we dont need to allocate memory for server address since it lives in main
    // thread and main thread ends after all threads end, thus it is safe to
    // use it
    AddressWrapper server_address;
    AddressWrapper my_address;
    std::shared_ptr<ClientFdWrapper> client_fd_sp;
    std::mutex mutex_player;
};

#endif // PLAYER_CLASS_H