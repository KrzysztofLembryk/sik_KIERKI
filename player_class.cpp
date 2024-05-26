#include "player_class.h"

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