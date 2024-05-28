#ifndef GAME_MASTER_H
#define GAME_MASTER_H

#include "player_class.h"
#include <memory>
#include <mutex>
#include <semaphore>

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
                                        const struct sockaddr_in6 &server_addr);
        ~GameMaster() = default;

        bool check_if_position_taken(PlayerPosition pos);
        std::vector<PlayerPosition> get_taken_positions();
        void add_new_player(PlayerPosition pos, 
                            struct sockaddr_in6 &my_address);
        
        void wait_for_turn(PlayerPosition pos);

        PlayerPosition get_whose_turn();
        GameType get_game_type();


        cardCls::DeckOfCards get_player_cards(PlayerPosition pos);
        std::shared_ptr<Player> get_player(PlayerPosition pos);


    private:
        std::map<PlayerPosition, std::binary_semaphore> semaphore_map;
        std::map<PlayerPosition, std::shared_ptr<Player>> players;
        std::vector<gameCls::Round> rounds;
        cardCls::Lewa curr_lewa;
        PlayerPosition whose_turn;
        uint8_t round_number;
        gameCls::CardCounter card_counter;
        std::map<PlayerPosition, bool> pos_taken_map;
        uint8_t number_of_players_present;
        std::mutex mutex_gm;
    };
}

#endif