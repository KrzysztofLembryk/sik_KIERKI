#include "game_master.h"
#include "constants.h"
#include <arpa/inet.h>

gm::GameMaster::GameMaster(std::vector<gameCls::Round> &rounds, 
    struct sockaddr_in6 &server_addr) : 
pos_taken_map({{N, false}, {E, false}, {S, false}, {W, false}}), 
sync_barrier(MAX_PLAYERS, [](){})
{
    this->semaphore_map[N] = std::make_shared<std::binary_semaphore>(0);
    this->semaphore_map[E] = std::make_shared<std::binary_semaphore>(0);
    this->semaphore_map[S] = std::make_shared<std::binary_semaphore>(0);
    this->semaphore_map[W] = std::make_shared<std::binary_semaphore>(0);
    this->curr_lewa = std::make_shared<cardCls::Lewa>(1);
    this->rounds = rounds;
    this->round_number = 1;
    this->first_player = rounds[0].get_first_player();
    this->whose_turn = first_player;
    this->who_won_lewa = NONE_POS;
    this->semaphore_map[first_player]->release();
    this->card_counter.new_game(rounds[0].get_game_type());
    this->number_of_players_present = 0;
    this->is_game_started = false;
    this->sem_print_msg = std::make_shared<std::binary_semaphore>(1);

    std::vector<PlayerPosition> player_pos({N, E, S, W});
    for (auto pos : player_pos)
    {
        this->players[pos] = std::make_shared<Player>(rounds[0].get_player_cards(pos), pos, rounds[0].get_game_type(), (struct sockaddr*)&server_addr);
    }
    // struct sockaddr *base_addr = (struct sockaddr*)&server_addr;

    // struct sockaddr_in6 *base_addr_6in = (struct sockaddr_in6*)base_addr;

    // char ip_str[INET6_ADDRSTRLEN];
    // inet_ntop(AF_INET6, &(base_addr_6in->sin6_addr), ip_str, INET6_ADDRSTRLEN);
    // std::cout << "GameMaster constructor- main - server_address in game_master: " << ip_str << ":" << ntohs(base_addr_6in->sin6_port) << "\n"; 

    // char s_ip_str[INET6_ADDRSTRLEN];
    // inet_ntop(AF_INET6, &(server_addr.sin6_addr), s_ip_str, INET6_ADDRSTRLEN);
    // std::cout << "GameMaster constructor- main - server_address in game_master: " << s_ip_str << ":" << ntohs(server_addr.sin6_port) << "\n"; 
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
void gm::GameMaster::add_new_player(PlayerPosition pos, struct sockaddr_in6 &p_address)
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    pos_taken_map[pos] = true;
    struct sockaddr_in6 *p_address_alloc = new struct sockaddr_in6(p_address);
    players[pos]->set_player_address((struct sockaddr*)&p_address_alloc);

    // Only at the beginning when we add new players we count them and set 
    // is_game_started to true, after that when player leaves we dont change 
    // nbr_of_players_present since game is already started
    if (number_of_players_present < MAX_PLAYERS)
        number_of_players_present++;

    if (number_of_players_present == MAX_PLAYERS)
    {
        is_game_started = true;
    }
}

void gm::GameMaster::add_card_to_lewa(cardCls::CardClassWrapper &card)
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    curr_lewa->add_card(card);
}

/**
 * @brief Function acquires semaphore_map at pos, at a time only one of these
 * semaphores is open, since only one player can play at a time.
*/
void gm::GameMaster::wait_for_turn(PlayerPosition pos)
{
    semaphore_map[pos]->acquire();    
}

void gm::GameMaster::wait_for_game_start()
{
    sync_barrier.arrive_and_wait();
}

void gm::GameMaster::wait_for_all_players()
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
    return rounds[round_number - 1].get_game_type();
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

uint8_t gm::GameMaster::get_nbr_of_rounds()
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    return (uint8_t)rounds.size();
}

cardCls::DeckOfCards gm::GameMaster::get_player_deck(PlayerPosition pos)
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    return rounds[round_number - 1].get_player_cards(pos);
}

PlayerPosition gm::GameMaster::get_who_won_lewa()
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    return who_won_lewa;
}

std::map<PlayerPosition, uint8_t> gm::GameMaster::get_player_scores()
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    std::map<PlayerPosition, uint8_t> scores;
    for (auto &player : players)
    {
        scores[player.first] = player.second->get_curr_score();
    }
    return scores;
}

std::map<PlayerPosition, uint32_t> gm::GameMaster::get_player_all_points()
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    std::map<PlayerPosition, uint32_t> all_points;
    for (auto &player : players)
    {
        all_points[player.first] = player.second->get_all_points();
    }
    return all_points;
}

bool gm::GameMaster::check_if_game_started()
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    return is_game_started;
}

bool gm::GameMaster::check_if_curr_lewa_full()
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    return this->curr_lewa->lewa_full();
}

void gm::GameMaster::check_who_won_lewa()
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    auto lewas_cards = curr_lewa->get_cards_in_lewa();
    Suit bottom_card_suit = lewas_cards[0].get_suit();
    int i = 1;
    cardCls::CardClassWrapper winner_card = lewas_cards[0];
    PlayerPosition winner = this->first_player;
    while (i < MAX_LEWA_SIZE)
    {
        if (lewas_cards[i].get_suit() == bottom_card_suit)
        {
            if (lewas_cards[i].get_value() > winner_card.get_value())
            {
                winner_card = lewas_cards[i];
                winner = static_cast<PlayerPosition>((this->first_player + i) % MAX_PLAYERS);
            }
        }
        i++;
    }

    this->who_won_lewa = winner;
    curr_lewa->set_player_who_took_lewa(winner);
}

bool gm::GameMaster::check_if_round_finished()
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    return card_counter.has_game_ended();
}

bool gm::GameMaster::check_if_last_round()
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    return round_number == rounds.size();
}

void gm::GameMaster::count_cards_played()
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    
    card_counter.count_cards(*curr_lewa);
}

void gm::GameMaster::next_player_turn()
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    whose_turn = static_cast<PlayerPosition>((whose_turn + 1) % MAX_PLAYERS);
    semaphore_map[whose_turn]->release();
}

void gm::GameMaster::set_first_player_for_next_turn()
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    // We need to set first player and whose turn to winner, since he will be 
    // playing first next card, also we need to release his semaphore.
    this->first_player = who_won_lewa;
    this->whose_turn = who_won_lewa;
    semaphore_map[whose_turn]->release();

}

int gm::GameMaster::decrement_present_players()
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    if (number_of_players_present > 0)
        number_of_players_present--;
    else 
        exception_wrappers::runtime_err_wrapper("Wanted to decrement Nbr of players present when it is 0");
    return number_of_players_present;
}

void gm::GameMaster::prepare_new_lewa()
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    if (curr_lewa->lewa_full())
    {
        lewas_played.push_back(*curr_lewa);
        curr_lewa->clear_lewa();
        curr_lewa->set_lewa_id(curr_lewa->get_lewa_id() + 1);
    }
}

/**
 * @brief Only first player who calls this function modifies lewa, other threads
 * dont go into if since lewa is cleared and its size is 0
*/
void gm::GameMaster::prepare_new_round()
{
    std::lock_guard<std::mutex> lock(mutex_gm);
    if (curr_lewa->lewa_full())
    {
        curr_lewa->clear_lewa();
        curr_lewa->set_lewa_id(1);
        lewas_played.clear();
        round_number++;
        // Since in function that checks who took lewa we release semaphore for
        // player who took it, if round has ended we need to acquire it
        semaphore_map[whose_turn]->acquire();

        first_player = rounds[round_number - 1].get_first_player();
        whose_turn = first_player;
        who_won_lewa = NONE_POS;
        semaphore_map[first_player]->release();
        card_counter.new_game(rounds[round_number - 1].get_game_type());
    }
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

void gm::GameMaster::acquire_print_msg_sem()
{
    sem_print_msg->acquire();
}

void gm::GameMaster::release_print_msg_sem()
{
    sem_print_msg->release();
}