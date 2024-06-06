#include "player_class.h"
#include "constants.h"

void Player::set_hand(cardCls::DeckOfCards &hand)
{
    this->hand = hand;
}

void Player::set_player_address(const struct sockaddr_in6 &addr)
{
    this->my_address = addr;
}

void Player::set_card_played(cardCls::CardClassWrapper &card)
{
    hand.set_card_played(card);
}

void Player::set_game_type(GameType game_type)
{
    this->point_counter.set_game_type(game_type);
}

cardCls::DeckOfCards Player::get_hand()
{
    return hand;
}

PlayerPosition Player::get_position()
{
    return position;
}

int Player::check_card_correctness(cardCls::CardClassWrapper &card, 
    Suit bottom_card_suit)
{
    try 
    {
        // We check if given card was played or if it is in deck. 
        // If it was played or if is not in deck we return error
        if (this->hand.was_card_played(card))
        {
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
    uint8_t new_points = this->point_counter.count_points(lewa);
    this->curr_game_points += new_points;
}

void Player::add_points_from_round_to_allpoints()
{
    this->all_points += (uint32_t)(this->curr_game_points);
    this->curr_game_points = 0;
}