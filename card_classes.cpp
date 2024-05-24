#include "card_classes.h"
#include "constants.h"
#include <iostream>
#include "exception_wrappers.h"

// CARD CLASS WRAPPER
deck::CardClassWrapper::CardClassWrapper(Suit suit, uint8_t value)
{
    this->card.suit = suit;
    if (value >= 2 && value <= 10)
    {
        this->card.value = value;
    }
    else if (value >= J && value <= A)
    {
        this->card.value = value;
    }
    else
    {
        exception_wrappers::invalid_arg_wrapper("Invalid card value");
    }
}

bool deck::CardClassWrapper::operator==(const CardClassWrapper &other) const
{
    return card.suit == other.card.suit && card.value == other.card.value;
}

bool deck::CardClassWrapper::operator<(const CardClassWrapper &other) const
{
    return card.suit < other.card.suit || (card.suit == other.card.suit && card.value < other.card.value);
}

Suit deck::CardClassWrapper::get_suit() const
{
    return card.suit;
}

uint8_t deck::CardClassWrapper::get_value() const
{
    return card.value;
}

void deck::CardClassWrapper::print() const
{
    if (card.value >= 2 && card.value <= 10)
    {
        std::cout << unsigned(card.value);
    }
    else
    {
        switch (card.value)
        {
        case J:
            std::cout << "J";
            break;
        case Q:
            std::cout << "Q";
            break;
        case K:
            std::cout << "K";
            break;
        case A:
            std::cout << "A";
            break;
        default:
            exception_wrappers::invalid_arg_wrapper("Invalid card value");
        }
    }
    std::cout << (char)card.suit;
}


// DECK OF CARDS
void deck::DeckOfCards::add_card(const CardClassWrapper &card)
{
    if (was_card_played_map.find(card) == was_card_played_map.end())
    {
        if (was_card_played_map.size() == MAX_CARDS_IN_DECK)
        {
            exception_wrappers::invalid_arg_wrapper("Deck is full, cannot add more cards");
        }
        was_card_played_map[card] = false;
    }
    else 
        exception_wrappers::invalid_arg_wrapper("Card already in deck");
}

void deck::DeckOfCards::set_card_played(const CardClassWrapper &card)
{
    if (was_card_played_map.find(card) != was_card_played_map.end())
    {
        if (was_card_played_map[card])
        {
            exception_wrappers::invalid_arg_wrapper("Card already played");
        }
        was_card_played_map[card] = true;
    }
    else 
        exception_wrappers::invalid_arg_wrapper("Card not in deck");
}

bool deck::DeckOfCards::was_card_played(const CardClassWrapper &card) const
{
    if (was_card_played_map.find(card) == was_card_played_map.end())
    {
        exception_wrappers::invalid_arg_wrapper("Card not in deck");
    }
    return was_card_played_map.at(card);
}

void deck::DeckOfCards::reset()
{
    for (auto &pair : was_card_played_map)
    {
        pair.second = false;
    }
}

void deck::DeckOfCards::print_deck() const
{
    for (const auto &pair : was_card_played_map)
    {
        pair.first.print();
        std::cout << " ";
    }
    std::cout << "\n";
}

// LEWA

void deck::Lewa::add_card(const CardClassWrapper &card)
{
    if (lewa.size() == MAX_LEWA_SIZE)
    {
        exception_wrappers::invalid_arg_wrapper("Lewa " +
                                                std::to_string(this->lewa_id) + " is full");
    }
    lewa.push_back(card);
}

void deck::Lewa::clear_lewa()
{
    lewa.clear();
}

const std::vector<deck::CardClassWrapper> &deck::Lewa::get_cards_in_lewa()
{
    return lewa;
}

int deck::Lewa::get_lewa_id() const
{
    return lewa_id;
}

bool deck::Lewa::lewa_full() const
{
    return lewa.size() == MAX_LEWA_SIZE;
}

void deck::Lewa::set_player_who_took_lewa(PlayerPosition player)
{
    if (player == NONE_POS)
    {
        exception_wrappers::invalid_arg_wrapper("Player who took lewa cannot be NONE_POS");
    }
    if (player_who_took_lewa != NONE_POS)
    {
        exception_wrappers::invalid_arg_wrapper("Player who took lewa already set");
    }
    player_who_took_lewa = player;
}
