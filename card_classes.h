#ifndef CARD_CLASSES_H
#define CARD_CLASSES_H

#include <map>
#include <stdexcept>
#include <vector>
#include <memory>
#include <source_location>
#include <iostream>
#include "constants.h"
#include "enum_types.h"

namespace deck
{
    // Card struct needed for C-style socket API, to send it by TCP socket
    typedef struct __attribute__((__packed__)) Card
    {
        Suit suit;
        uint8_t value;
    } Card;

    /**
     * @brief Wrapper for Card struct, to use it in C++ code (i.e. in std::map 
     * as key). Has get methods and operators for comparison.
    */
    class CardClassWrapper
    {
    public:
        // Constructors
        CardClassWrapper() = delete;
        CardClassWrapper(Suit suit, uint8_t value)
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
                throw std::invalid_argument("Invalid card value");
            }
        }
        CardClassWrapper(Card card) : card(card) {}

        // Destructor
        ~CardClassWrapper() = default;

        // Operators
        bool operator==(const CardClassWrapper &other) const
        {
            return card.suit == other.card.suit && card.value == other.card.value;
        }

        bool operator<(const CardClassWrapper &other) const
        {
            return card.suit < other.card.suit || (card.suit == other.card.suit && card.value < other.card.value);
        }

        Suit get_suit() const
        {
            return card.suit;
        }

        uint8_t get_value() const
        {
            return card.value;
        }

        void print() const
        {
            std::cout << "value: ";
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
                    throw std::invalid_argument("Invalid card value");
                }
            }
            std::cout << ", suit: " << (char)card.suit << " \n";
        }

    private:
        Card card;
        // Dont know yet whether Card should know if it was played
    };

    /**
     * @brief DeckOfCards class stores Cards objects in a map. It has methods to
     * add card to deck, set card as played and check if card was played.
    */
    class DeckOfCards
    {
    public:
        DeckOfCards(bool init = false)
        {
            if (init)
            {
                for (uint8_t i = 2; i <= 10; i++)
                {
                    was_card_played_map[{HEARTS, i}] = false;
                    was_card_played_map[{DIAMONDS, i}] = false;
                    was_card_played_map[{CLUBS, i}] = false;
                    was_card_played_map[{SPADES, i}] = false;
                }
                for (uint8_t i = J; i <= A; i++)
                {
                    was_card_played_map[{HEARTS, i}] = false;
                    was_card_played_map[{DIAMONDS, i}] = false;
                    was_card_played_map[{CLUBS, i}] = false;
                    was_card_played_map[{SPADES, i}] = false;
                }
            }
        }
        ~DeckOfCards() = default;

        void add_card(const CardClassWrapper &card)
        {
            if (was_card_played_map.find(card) == was_card_played_map.end())
            {
                if (was_card_played_map.size() == MAX_CARDS_IN_DECK)
                {
                    throw std::invalid_argument(": Deck is full, cannot add more cards");
                }
                was_card_played_map[card] = false;
            }
            else 
                throw std::invalid_argument("Card already in deck");
        }

        void set_card_played(const CardClassWrapper &card)
        {
            if (was_card_played_map.find(card) != was_card_played_map.end())
            {
                if (was_card_played_map[card])
                {
                    throw std::invalid_argument("Card already played");
                }
                was_card_played_map[card] = true;
            }
            else 
                throw std::invalid_argument("Card not in deck");
        }

        bool was_card_played(const CardClassWrapper &card) const
        {
            if (was_card_played_map.find(card) == was_card_played_map.end())
            {
                throw std::invalid_argument("Card not in deck");
            }
            return was_card_played_map.at(card);
        }

        void reset()
        {
            for (auto &pair : was_card_played_map)
            {
                pair.second = false;
            }
        }

        void print() const
        {
            for (const auto &pair : was_card_played_map)
            {
                if (pair.first.get_value() >= 2 && pair.first.get_value() <= 10)
                {
                    std::cout << pair.first.get_value();
                }
                else
                {
                    switch (pair.first.get_value())
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
                        throw std::invalid_argument("Invalid card value");
                    }
                }
                std::cout << pair.first.get_suit() << " ";
            }
            std::cout << "\n";
        }

    private:
        std::map<CardClassWrapper, bool> was_card_played_map;
    };

    /**
     * @brief Trik == Lewa in polish, I prefer to use polish name for it.
     * Lewa class stores maximally 4 cards, which are played in one round.
     * It has methods to add card to lewa and get vector of cards in lewa.
    */
    class Lewa
    {
    public:
        Lewa(int nbr) : lewa_id(nbr), player_who_took_lewa(NONE_POS) {}
        ~Lewa() = default;

        void add_card(const CardClassWrapper &card)
        {
            if (lewa.size() == MAX_LEWA_SIZE)
            {
                throw std::invalid_argument("Lewa " +
                                            std::to_string(this->lewa_id) + " is full");
            }
            lewa.push_back(card);
        }

        void clear_lewa()
        {
            lewa.clear();
        }

        const std::vector<CardClassWrapper> &get_cards_in_lewa()
        {
            return lewa;
        }

        int get_lewa_id() const
        {
            return lewa_id;
        }

        bool id_lewa_full() const
        {
            return lewa.size() == MAX_LEWA_SIZE;
        }

        void set_player_who_took_lewa(PlayerPosition player)
        {
            if (player == NONE_POS)
            {
                throw std::invalid_argument("Player who took lewa cannot be NONE_POS");
            }
            if (player_who_took_lewa != NONE_POS)
            {
                throw std::invalid_argument("Player who took lewa already set");
            }
            player_who_took_lewa = player;
        }

    private:
        int lewa_id;
        std::vector<CardClassWrapper> lewa;
        PlayerPosition player_who_took_lewa;
    };
 
} // namespace deck

#endif // CARD_CLASSES_H