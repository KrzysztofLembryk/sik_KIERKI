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

/**
 * @brief Counts points for given game type
 * There are seven game types:
 * - NO_LEWA - 1 point for getting a Lewa
 * - NO_HEART - 1 point for each heart
 * - NO_QUEEN - 5 points for each queen
 * - NO_MISTER - 2 points for each king and jack
 * - NO_KING_HEART - 18 points for king of hearts
 * - NO_SEVEN_AND_LAST - 10 points for seventh and thirteenth lewa
 * - BANDIT - sum of all above
 */
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
        points = count_figures(K, 2, lewa);
        points += count_figures(J, 2, lewa);
        break;
    case NO_KING_HEART:
        points = count_figures(K, 18, lewa, true);
        break;
    case NO_SEVEN_AND_LAST:
        if (lewa.get_lewa_id() == 7 || lewa.get_lewa_id() == 13)
            points = 10;
        break;
    case BANDIT:
        points = 1;
        points += count_colors(HEARTS, 1, lewa);
        points += count_figures(Q, 5, lewa);
        points += count_figures(K, 2, lewa);
        points += count_figures(J, 2, lewa);
        points += count_figures(K, 18, lewa, true);
        if (lewa.get_lewa_id() == 7 || lewa.get_lewa_id() == 13)
            points += 10;
        break;
    }
    return points;
}

void CardCounter::count_cards(deck::Lewa &lewa)
{
    auto cards = lewa.get_cards_in_lewa();

    for (auto card : cards)
    {
        if (card.get_value() == Q)
        {
            this->nbr_of_queens_played++;
        }
        else if (card.get_value() == K)
        {
            this->nbr_of_misters_played++;
        }
        else if (card.get_value() == J)
        {
            this->nbr_of_misters_played++;
        }
        else if (card.get_value() == K && card.get_suit() == HEARTS)
        {
            this->king_of_hearts_played = true;
        }
        else if (card.get_suit() == HEARTS)
        {
            this->nbr_of_hearts_played++;
        }
    }

    this->nbr_of_lewas_played++;

    if (this->nbr_of_lewas_played == 13)
    {
        this->seven_and_last_played = true;
    }
}

bool CardCounter::has_game_ended()
{
    switch (this->game_type)
    {
    case NO_LEWA:
        return this->nbr_of_lewas_played == 13;
    case NO_HEART:
        return this->nbr_of_hearts_played == 13;
    case NO_QUEEN:
        return this->nbr_of_queens_played == 4;
    case NO_MISTER:
        return this->nbr_of_misters_played == 8;
    case NO_KING_HEART:
        return this->king_of_hearts_played;
    case NO_SEVEN_AND_LAST:
        return this->seven_and_last_played;
    case BANDIT:
        return this->nbr_of_lewas_played == 13;     
    }
    return false;
}
