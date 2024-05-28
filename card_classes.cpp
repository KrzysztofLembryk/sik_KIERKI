#include "card_classes.h"
#include "constants.h"
#include <iostream>
#include "exception_wrappers.h"
#include "enum_types.h"
#include <vector>


std::vector<char> cardVec_to_charVec(std::vector<cardCls::CardClassWrapper> cards)
{
    std::vector<char> deck;
    for (const auto &card : cards)
    {
        uint8_t value = card.get_value();
        if (value >= 2 && value < 10)
        {
            deck.push_back(std::to_string(value)[0]);
            std::cout << "value of card in cardVec_to_charVec: " << std::to_string(value)[0] << "\n";
        }
        else if (value == 10)
        {
            deck.push_back('1');
            deck.push_back('0');
        }
        else
        {
            switch (value)
            {
            case J:
                deck.push_back('J');
                break;
            case Q:
                deck.push_back('Q');
                break;
            case K:
                deck.push_back('K');
                break;
            case A:
                deck.push_back('A');
                break;
            default:
                exception_wrappers::invalid_arg_wrapper("Invalid card value");
            }
        }
        deck.push_back(card.get_suit());
    }
    return deck;
}






// CARD CLASS WRAPPER
cardCls::CardClassWrapper::CardClassWrapper(Suit suit, uint8_t value)
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

bool cardCls::CardClassWrapper::operator==(const CardClassWrapper &other) const
{
    return card.suit == other.card.suit && card.value == other.card.value;
}

bool cardCls::CardClassWrapper::operator<(const CardClassWrapper &other) const
{
    return card.suit < other.card.suit || (card.suit == other.card.suit && card.value < other.card.value);
}

Suit cardCls::CardClassWrapper::get_suit() const
{
    return card.suit;
}

uint8_t cardCls::CardClassWrapper::get_value() const
{
    return card.value;
}

void cardCls::CardClassWrapper::print() const
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
void cardCls::DeckOfCards::add_card(const CardClassWrapper &card)
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

void cardCls::DeckOfCards::set_card_played(const CardClassWrapper &card)
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

bool cardCls::DeckOfCards::was_card_played(const CardClassWrapper &card) const
{
    if (was_card_played_map.find(card) == was_card_played_map.end())
    {
        exception_wrappers::invalid_arg_wrapper("Card not in deck");
    }
    return was_card_played_map.at(card);
}

void cardCls::DeckOfCards::reset()
{
    for (auto &pair : was_card_played_map)
    {
        pair.second = false;
    }
}

void cardCls::DeckOfCards::print_deck() const
{
    for (const auto &pair : was_card_played_map)
    {
        pair.first.print();
        std::cout << " ";
    }
    std::cout << "\n";
}


std::vector<char> cardCls::DeckOfCards::to_char_vector() const
{
    std::vector<cardCls::CardClassWrapper> cardVec;
    for (const auto &pair : was_card_played_map)
    {
        cardVec.push_back(pair.first);
    }
    std::vector<char> deck = cardVec_to_charVec(cardVec);
    return deck;
}

size_t cardCls::DeckOfCards::size() const
{
    return was_card_played_map.size();
}

// LEWA

void cardCls::Lewa::add_card(const CardClassWrapper &card)
{
    if (lewa.size() == MAX_LEWA_SIZE)
    {
        exception_wrappers::invalid_arg_wrapper("Lewa " +
                                                std::to_string(this->lewa_id) + " is full");
    }
    lewa.push_back(card);
}

void cardCls::Lewa::clear_lewa()
{
    lewa.clear();
}

const std::vector<cardCls::CardClassWrapper> &cardCls::Lewa::get_cards_in_lewa()
{
    return lewa;
}

uint8_t cardCls::Lewa::get_lewa_id() const
{
    return lewa_id;
}

void cardCls::Lewa::set_lewa_id(uint8_t id)
{
    lewa_id = id;
}

void cardCls::Lewa::set_lewa_id(const std::vector<char> &id)
{
    if (id.size() == 1)
    {
        lewa_id = id[0] - '0';
    }
    else if (id.size() == 2)
    {
        lewa_id = (id[0] - '0') * 10 + (id[1] - '0');
    }
    else
    {
        exception_wrappers::invalid_arg_wrapper("Invalid lewa id size");
    }

    if (lewa_id < 1 || lewa_id > 13)
    {
        exception_wrappers::invalid_arg_wrapper("Invalid lewa id value");
    }
}

bool cardCls::Lewa::lewa_full() const
{
    return lewa.size() == MAX_LEWA_SIZE;
}

size_t cardCls::Lewa::size() const
{
    return lewa.size();
}

void cardCls::Lewa::set_player_who_took_lewa(PlayerPosition player)
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


std::vector<char> cardCls::Lewa::to_char_vector() const
{
    return cardVec_to_charVec(this->lewa);
}


std::vector<char> cardCls::Lewa::get_lewa_id_as_char() const
{
    if (lewa_id < 10)
    {
        return {static_cast<char>(lewa_id + '0')};
    }
    else
    {
        std::string id = std::to_string(lewa_id);
        return {id[0], id[1]};
    }
}

cardCls::CardClassWrapper cardCls::Lewa::get_top_card() const
{
    if (lewa.size() == 0)
    {
        exception_wrappers::invalid_arg_wrapper("Lewa " +
                                                std::to_string(this->lewa_id) + " is empty");
    }
    return lewa.back();
}

void cardCls::Lewa::print()
{
    for (const auto &card : lewa)
    {
        card.print();
        std::cout << " ";
    }
    std::cout << "\n";
}