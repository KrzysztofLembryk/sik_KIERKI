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

cardCls::DeckOfCards Player::get_hand()
{
    return hand;
}

PlayerPosition Player::get_position()
{
    return position;
}

int Player::check_card_correctness(cardCls::CardClassWrapper &card)
{
    try 
    {
        if (this->hand.was_card_played(card))
        {
            return ERROR;
        }
        return SUCCESS;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return ERROR;
    }
}