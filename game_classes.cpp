#include "game_classes.h"

using namespace game;

uint8_t count_figures(FigureCardValue figure, uint8_t val, deck::Lewa &lewa, bool is_king_heart = false)
{
    uint8_t points = 0;
    auto cards = lewa.get_cards_in_lewa();

    for (auto card : cards)
    {
        if (is_king_heart)
        {
            if (card.get_value() == K && card.get_suit() == HEARTS)
            {
                points += val;
                return points;
            }
        }
        else if (card.get_value() == figure)
        {
            points += val;
        }
    }
    return points;
}

uint8_t count_colors(Suit suit, uint8_t val, deck::Lewa &lewa)
{
    uint8_t points = 0;
    auto cards = lewa.get_cards_in_lewa();

    for (auto card : cards)
    {
        if (card.get_suit() == suit)
        {
            points += val;
        }
    }
    return points; 
}


uint8_t PointCounter::count_points(deck::Lewa &lewa)
{
    uint8_t points = 0;
    switch (this->game_type)
    {
        case NO_LEWA:
            points = 1;
            break;
        case NO_HEART:
            points = count_colors(HEARTS, 1, lewa);
            break;
        case NO_QUEEN:
            points = count_figures(Q, 5, lewa);
            break;
        case NO_MISTER:
            // points = count_figures(K, 5, lewa);
            break;
        case NO_KING_HEART:
            return 0;
            break;
        case NO_SEVEN_AND_LAST:
            return 0;
            break;
        case BANDIT:
            return 0;
            break;
    }
    return points;
}