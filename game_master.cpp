#include "game_master.h"
#include "constants.h"

gm::GameMaster::GameMaster(std::vector<gameCls::Round> &rounds, const struct sockaddr_in6 &server_addr) :
pos_taken_map({{N, false}, {E, false}, {S, false}, {W, false}}), sync_barrier(MAX_PLAYERS, [](){})
{
    this->semaphore_map.emplace(N, std::binary_semaphore(0));
    this->semaphore_map.emplace(E, std::binary_semaphore(0));
    this->semaphore_map.emplace(S, std::binary_semaphore(0));
    this->semaphore_map.emplace(W, std::binary_semaphore(0));

    this->curr_lewa = std::make_shared<cardCls::Lewa>(1);
    this->rounds = rounds;
    this->round_number = 1;
    this->whose_turn = rounds[0].get_first_player();
    this->semaphore_map[whose_turn].release();
    this->card_counter.new_game(rounds[0].get_game_type());
    this->number_of_players_present = 0;
    this->is_game_started = false;

    std::vector<PlayerPosition> player_pos({N, E, S, W});

    for (auto pos : player_pos)
    {
        this->players[pos] = std::make_shared<Player>(rounds[0].get_player_cards(pos), pos, rounds[0].get_game_type(), server_addr);
    }
}

bool gm::GameMaster::check_if_position_taken(PlayerPosition pos)
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    return pos_taken_map[pos];
}

std::vector<PlayerPosition> gm::GameMaster::get_taken_positions()
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    std::vector<PlayerPosition> taken_seats;

    for (auto &seat : pos_taken_map)
    {
        if (seat.second)
        {
            taken_seats.push_back(seat.first);
        }
    }

    return taken_seats;
}

/**
 * @brief Critical section- acquires mutex_gm, sets pos_taken_map[pos] to true, 
 * increments number_of_players_present and if its equal to 4 it releases 4 
 * permits on barrier_first, meaning game can start.
*/
void gm::GameMaster::add_new_player(PlayerPosition pos, struct sockaddr_in6 &my_address)
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    pos_taken_map[pos] = true;
    players[pos]->set_player_address(my_address);
    number_of_players_present++;

    if (number_of_players_present == MAX_PLAYERS)
    {
        is_game_started = true;
    }
}

/**
 * @brief Function acquires semaphore_map at pos, at a time only one of these
 * semaphores is open, since only one player can play at a time.
*/
void gm::GameMaster::wait_for_turn(PlayerPosition pos)
{
    semaphore_map[pos].acquire();    
}

void gm::GameMaster::wait_for_game_start()
{
    sync_barrier.arrive_and_wait();
}


PlayerPosition gm::GameMaster::get_whose_turn() 
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    return whose_turn;
}

GameType gm::GameMaster::get_game_type()
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    return rounds[round_number].get_game_type();
}

std::shared_ptr<cardCls::Lewa> gm::GameMaster::get_curr_lewa()
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    return curr_lewa;
}

uint8_t gm::GameMaster::get_curr_round_nbr()
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    return round_number;
}

bool gm::GameMaster::check_if_game_started()
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    return is_game_started;
}

// cardCls::DeckOfCards gm::GameMaster::get_player_cards(PlayerPosition pos)
// {
//     std::lock_guard<std::mutex> lock(mutex_gm);
//     return players[pos]->get_hand();
// }

std::shared_ptr<Player> gm::GameMaster::get_player(PlayerPosition pos)
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    return players[pos];
}