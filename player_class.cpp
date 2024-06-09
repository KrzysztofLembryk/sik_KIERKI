#include "player_class.h"
#include "constants.h"

void Player::set_hand(cardCls::DeckOfCards &hand)
{
    std::lock_guard<std::mutex> lock(mutex_player);
    this->hand = hand;
}

void Player::set_player_address(struct sockaddr_in &addr)
{
    std::lock_guard<std::mutex> lock(mutex_player);
    my_address.set_address(addr);
}

void Player::set_player_address(struct sockaddr_in6 &addr)
{
    std::lock_guard<std::mutex> lock(mutex_player);
    my_address.set_address(addr);
}

void Player::set_player_address(AddressWrapper &addr)
{
    std::lock_guard<std::mutex> lock(mutex_player);
    my_address = addr;
}

void Player::set_server_address(struct sockaddr_in &addr)
{
    std::lock_guard<std::mutex> lock(mutex_player);
    server_address.set_address(addr);
}

void Player::set_server_address(struct sockaddr_in6 &addr)
{
    std::lock_guard<std::mutex> lock(mutex_player);
    server_address.set_address(addr);
}

void Player::set_server_address(AddressWrapper &addr)
{
    std::lock_guard<std::mutex> lock(mutex_player);
    server_address = addr;
}

void Player::set_client_fd(std::shared_ptr<ClientFdWrapper> client_fd_sp)
{
    std::lock_guard<std::mutex> lock(mutex_player);
    this->client_fd_sp = client_fd_sp;
}

void Player::set_card_played(cardCls::CardClassWrapper &card)
{
    std::lock_guard<std::mutex> lock(mutex_player);
    hand.set_card_played(card);
}

void Player::set_curr_lewa_bottom_suit(Suit bottom_suit)
{
    std::lock_guard<std::mutex> lock(mutex_player);
    curr_lewa_bottom_suit = bottom_suit;
}

void Player::set_chosen_card_by_human_player(cardCls::CardClassWrapper &card)
{
    std::lock_guard<std::mutex> lock(mutex_player);
    chosen_card_by_human_player = card;
}

void Player::set_game_type(GameType game_type)
{
    std::lock_guard<std::mutex> lock(mutex_player);
    this->point_counter.set_game_type(game_type);
}

void Player::set_card_played(cardCls::Lewa &lewa)
{
    std::lock_guard<std::mutex> lock(mutex_player);
    hand.set_card_played(lewa);
}

cardCls::DeckOfCards Player::get_hand()
{
    std::lock_guard<std::mutex> lock(mutex_player);
    return hand;
}

PlayerPosition Player::get_position()
{
    std::lock_guard<std::mutex> lock(mutex_player);
    return position;
}

uint8_t Player::get_curr_score()
{
    std::lock_guard<std::mutex> lock(mutex_player);
    return curr_game_points;
}

uint32_t Player::get_all_points()
{
    std::lock_guard<std::mutex> lock(mutex_player);
    return all_points;
}

struct sockaddr* Player::get_server_address()
{
    std::lock_guard<std::mutex> lock(mutex_player);
    return server_address.get_address();
}

struct sockaddr* Player::get_client_address()
{
    std::lock_guard<std::mutex> lock(mutex_player);
    return my_address.get_address();
}

int Player::get_client_fd()
{
    std::lock_guard<std::mutex> lock(mutex_player);
    return client_fd_sp->to_int();
}

Suit Player::get_curr_lewa_bottom_suit()
{
    std::lock_guard<std::mutex> lock(mutex_player);
    return curr_lewa_bottom_suit;
}

cardCls::CardClassWrapper Player::get_chosen_card_by_human_player()
{
    std::lock_guard<std::mutex> lock(mutex_player);
    return chosen_card_by_human_player;
}

int Player::check_card_correctness(cardCls::CardClassWrapper &card, 
    Suit bottom_card_suit)
{
    std::lock_guard<std::mutex> lock(mutex_player);
    try 
    {
        // We check if given card was played or if it is in deck. 
        // If it was played or if is not in deck we return error
        if (this->hand.was_card_played(card))
        {
            std::cout << "ERROR: Card was already played\n";
            return ERROR;
        }
        
        // If card is in player's hand and has not yet been played we check if
        // it has correct suit, meaning suit of the bottom card in lewa
        if (card.get_suit() == bottom_card_suit)
        {
            return SUCCESS;
        }
        else 
        {
            // Here we check if player didnt cheat, meaning if he played card 
            // that has wrong suit even though he has card with correct suit
            if (hand.check_if_suit_available(bottom_card_suit))
            {
                return ERROR;
            }

        }
        return SUCCESS;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return ERROR;
    }
}

void Player::add_points_in_curr_round(cardCls::Lewa &lewa)
{
    std::lock_guard<std::mutex> lock(mutex_player);
    uint8_t new_points = this->point_counter.count_points(lewa);
    this->curr_game_points += new_points;
}

void Player::add_points_from_round_to_allpoints()
{
    std::lock_guard<std::mutex> lock(mutex_player);
    this->all_points += (uint32_t)(this->curr_game_points);
}

void Player::add_lewa_to_lewas_taken(cardCls::Lewa &lewa)
{
    std::lock_guard<std::mutex> lock(mutex_player);
    lewas_taken.push_back(lewa);
}

void Player::clear_lewas_taken()
{
    std::lock_guard<std::mutex> lock(mutex_player);
    lewas_taken.clear();
}

void Player::zero_curr_SCORE()
{
    std::lock_guard<std::mutex> lock(mutex_player);
    curr_game_points = 0;
}

void Player::reset_client_fd()
{
    std::lock_guard<std::mutex> lock(mutex_player);
    // we end connection with client
    client_fd_sp.reset();
}

cardCls::CardClassWrapper Player::play_card(Suit bottom_card_suit)
{
    std::lock_guard<std::mutex> lock(mutex_player);
    return hand.play_card(bottom_card_suit);
}

void Player::print_available_cards()
{
    std::lock_guard<std::mutex> lock(mutex_player);
    hand.print_available_cards();
    fflush(stdout);
}

void Player::print_taken_lewas()
{
    std::lock_guard<std::mutex> lock(mutex_player);
    for (auto &lewa : lewas_taken)
    {
        lewa.print();
        std::cout << '\n';
    }
    fflush(stdout);
}