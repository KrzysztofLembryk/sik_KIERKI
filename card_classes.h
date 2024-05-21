#ifndef CARD_CLASSES_H
#define CARD_CLASSES_H

#include <map>
#include <cstdint>
#include <vector>
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
        CardClassWrapper(Suit suit, uint8_t value);
        CardClassWrapper(Card card) : card(card) {}

        // Destructor
        ~CardClassWrapper() = default;

        // Operators
        bool operator==(const CardClassWrapper &other) const;

        bool operator<(const CardClassWrapper &other) const;

        Suit get_suit() const;

        uint8_t get_value() const;

        void print() const;

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
        DeckOfCards() = default;
        // {
        //     if (init)
        //     {
        //         for (uint8_t i = 2; i <= 10; i++)
        //         {
        //             was_card_played_map[{HEARTS, i}] = false;
        //             was_card_played_map[{DIAMONDS, i}] = false;
        //             was_card_played_map[{CLUBS, i}] = false;
        //             was_card_played_map[{SPADES, i}] = false;
        //         }
        //         for (uint8_t i = J; i <= A; i++)
        //         {
        //             was_card_played_map[{HEARTS, i}] = false;
        //             was_card_played_map[{DIAMONDS, i}] = false;
        //             was_card_played_map[{CLUBS, i}] = false;
        //             was_card_played_map[{SPADES, i}] = false;
        //         }
        //     }
        // }
        ~DeckOfCards() = default;

        void add_card(const CardClassWrapper &card);

        void set_card_played(const CardClassWrapper &card);

        bool was_card_played(const CardClassWrapper &card) const;

        void reset();

        void print() const;

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

        void add_card(const CardClassWrapper &card);

        void clear_lewa();

        const std::vector<CardClassWrapper> &get_cards_in_lewa();

        int get_lewa_id() const;

        bool id_lewa_full() const;

        void set_player_who_took_lewa(PlayerPosition player);

    private:
        int lewa_id;
        std::vector<CardClassWrapper> lewa;
        PlayerPosition player_who_took_lewa;
    };

} // namespace deck

#endif // CARD_CLASSES_H