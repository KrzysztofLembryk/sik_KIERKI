#ifndef GAME_MASTER_H
#define GAME_MASTER_H

#include "player_class.h"
#include <memory>
#include <mutex>
#include <semaphore>
#include <barrier>
#include "socket_fd_handler.h"

namespace gm
{
    /**
     * @brief GameMaster class is responsible for managing the game. 
     * It is a critical section, for threads, It is a MONITOR.
    */
    class GameMaster
    {
    public:
        GameMaster(std::vector<gameCls::Round> &rounds, 
                                        struct sockaddr_in6 &server_addr);
        ~GameMaster() = default;

        bool check_if_position_taken(PlayerPosition pos);
        std::vector<PlayerPosition> get_taken_positions();
        void set_player_left(PlayerPosition pos);

        int add_new_player(PlayerPosition pos, 
                            struct sockaddr_in6 &my_address,
                            std::shared_ptr<ClientFdWrapper> client_fd_sp);

        void add_card_to_lewa(cardCls::CardClassWrapper &card);
        
        void wait_for_turn(PlayerPosition pos);
        void wait_for_game_start();
        void wait_for_all_players();

        PlayerPosition get_whose_turn();
        PlayerPosition get_first_player();
        GameType get_game_type();
        std::shared_ptr<cardCls::Lewa> get_curr_lewa();
        size_t get_curr_round_nbr();
        uint8_t get_curr_lewa_nbr();
        size_t get_nbr_of_rounds();
        cardCls::DeckOfCards get_player_deck(PlayerPosition pos);
        PlayerPosition get_who_won_lewa();
        std::map<PlayerPosition, uint8_t> get_player_scores();
        std::map<PlayerPosition, uint32_t> get_player_all_points();
        std::vector<cardCls::Lewa> get_lewas_played();

        bool check_if_game_started();
        bool check_if_curr_lewa_full();
        void check_who_won_lewa();

        bool check_if_round_finished();
        bool check_if_last_round();

        void count_cards_played();
        void next_player_turn();
        void set_first_player_for_next_turn();
        int decrement_present_players();
        void prepare_new_lewa();
        void prepare_new_round();
        // cardCls::DeckOfCards get_player_cards(PlayerPosition pos);
        std::shared_ptr<Player> get_player(PlayerPosition pos);

        void acquire_print_msg_sem();
        void release_print_msg_sem();

        void acquire_disconnected_sem(PlayerPosition pos);

    private:
        std::map<PlayerPosition, std::shared_ptr<std::binary_semaphore>> semaphore_map;
        std::map<PlayerPosition, std::shared_ptr<Player>> players;
        std::vector<gameCls::Round> rounds;
        std::vector<cardCls::Lewa> lewas_played;
        std::shared_ptr<cardCls::Lewa> curr_lewa;
        PlayerPosition first_player;
        PlayerPosition whose_turn;
        PlayerPosition who_won_lewa;
        size_t round_number;
        uint8_t curr_lewa_nbr;
        gameCls::CardCounter card_counter;
        std::map<PlayerPosition, bool> pos_taken_map;
        uint8_t number_of_players_present;
        bool is_game_started;
        std::mutex mutex_gm;
        std::barrier<void(*)()> sync_barrier;
        std::shared_ptr<std::binary_semaphore> sem_print_msg;
        std::map<PlayerPosition, std::shared_ptr<std::binary_semaphore>> disconnected_sem_map;
    };
}

#endif