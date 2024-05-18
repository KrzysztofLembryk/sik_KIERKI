#ifndef CARD_STRUCTS_H
#define CARD_STRUCTS_H

#include <map>
#include <cstdint>
#include <stdexcept>

enum FigureCardValue
{
    J = 11,
    Q,
    K,
    A
};

enum Suit
{
    HEARTS = 'H',
    DIAMONDS = 'D',
    CLUBS = 'C',
    SPADES = 'S'
};
namespace deck
{
    typedef struct __attribute__((__packed__)) Card
    {
        Suit suit;
        uint8_t value;
    } Card;

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

    private:
        Card card;
    };

    class DeckOfCards
    {
    public:
        DeckOfCards(bool init = true) 
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

        bool add_card(const CardClassWrapper &card)
        {
            if (was_card_played_map.find(card) == was_card_played_map.end())
            {
                was_card_played_map[card] = false;
                return true;
            }
            return false;
        }

        void set_card_played(const CardClassWrapper &card)
        {
            was_card_played_map[card] = true;
        }

        bool was_card_played(const CardClassWrapper &card) const
        {
            return was_card_played_map.at(card);
        }

        void reset()
        {
            for (auto &pair : was_card_played_map)
            {
                pair.second = false;
            }
        }

    private:
        std::map<CardClassWrapper, bool> was_card_played_map;
    };
} // namespace deck

#endif // COMMUNICATION_STRUCTS_H