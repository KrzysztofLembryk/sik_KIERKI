#ifndef CARD_STRUCTS_H
#define CARD_STRUCTS_H

#include <map>
#include <cstdint>

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
        CardClassWrapper(Suit suit, uint8_t value) : card({suit, value}) {}
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

    class AllCards
    {
    public:
        // std::map<Suit, std::map<uint8_t, bool>> was_card_played_map;
        // albo
        std::map<CardClassWrapper, bool> was_card_played_map;
    };
} // namespace deck

#endif // COMMUNICATION_STRUCTS_H