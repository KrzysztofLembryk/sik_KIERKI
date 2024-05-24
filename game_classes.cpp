#include "game_classes.h"
#include "constants.h"
#include "exception_wrappers.h"
#include <iostream>

using namespace gameCls;

uint8_t count_figures(FigureCardValue figure, uint8_t val, cardCls::Lewa &lewa, bool is_king_heart = false)
{
    uint8_t points = INIT_POINTS;
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

uint8_t count_colors(Suit suit, uint8_t val, cardCls::Lewa &lewa)
{
    uint8_t points = INIT_POINTS;
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
uint8_t PointCounter::count_points(cardCls::Lewa &lewa)
{
    uint8_t points = INIT_POINTS;
    switch (this->game_type)
    {
    case NO_LEWA:
        points = NO_LEWA_POINTS;
        break;
    case NO_HEART:
        points = count_colors(HEARTS, NO_HEART_POINTS, lewa);
        break;
    case NO_QUEEN:
        points = count_figures(Q, NO_QUEEN_POINTS, lewa);
        break;
    case NO_MISTER:
        points = count_figures(K, NO_MISTER_POINTS, lewa);
        points += count_figures(J, NO_MISTER_POINTS, lewa);
        break;
    case NO_KING_HEART:
        points = count_figures(K, NO_KING_HEARTS_POINTS, lewa, true);
        break;
    case NO_SEVEN_AND_LAST:
        if (lewa.get_lewa_id() == 7 || lewa.get_lewa_id() == 13)
            points = NO_SEVEN_AND_LAST_POINTS;
        break;
    case BANDIT:
        points = NO_LEWA_POINTS;
        points += count_colors(HEARTS, NO_HEART_POINTS, lewa);
        points += count_figures(Q, NO_QUEEN_POINTS, lewa);
        points += count_figures(K, NO_MISTER_POINTS, lewa);
        points += count_figures(J, NO_MISTER_POINTS, lewa);
        points += count_figures(K, NO_KING_HEARTS_POINTS, lewa, true);

        if (lewa.get_lewa_id() == 7 || lewa.get_lewa_id() == 13)
            points += NO_SEVEN_AND_LAST_POINTS;
        break;
    }
    return points;
}

void PointCounter::set_game_type(GameType game_type)
{
    this->game_type = game_type;
}

void CardCounter::count_cards(cardCls::Lewa &lewa)
{
    if (!lewa.lewa_full())
    {
        exception_wrappers::invalid_arg_wrapper("Lewa is not full");
    }
    
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

    if (this->nbr_of_lewas_played == MAX_NBR_OF_LEWAS)
    {
        this->seven_and_last_played = true;
    }
}

bool CardCounter::has_game_ended()
{
    switch (this->game_type)
    {
    case NO_LEWA:
        return this->nbr_of_lewas_played == MAX_NBR_OF_LEWAS;
    case NO_HEART:
        return this->nbr_of_hearts_played == MAX_ONE_COLOR_SIZE;
    case NO_QUEEN:
        return this->nbr_of_queens_played == MAX_ONE_FIGURE_SIZE;
    case NO_MISTER:
        return this->nbr_of_misters_played == (2 * MAX_ONE_FIGURE_SIZE);
    case NO_KING_HEART:
        return this->king_of_hearts_played;
    case NO_SEVEN_AND_LAST:
        return this->seven_and_last_played;
    case BANDIT:
        return this->nbr_of_lewas_played == MAX_NBR_OF_LEWAS;
    }
    return false;
}

GameType Round::get_game_type() const
{
    return game_type;
}
PlayerPosition Round::get_first_player() const
{
    return first_player;
}
cardCls::DeckOfCards &Round::get_player_cards(PlayerPosition player_pos) 
{
    return player_cards.at(player_pos);
}

void Round::set_game_type(GameType game_type)
{
    this->game_type = game_type;
}

void Round::set_first_player(PlayerPosition first_player)
{
    this->first_player = first_player;
}

void Round::set_player_cards(PlayerPosition player_pos,
                             cardCls::DeckOfCards &cards)
{
    if (player_pos < 0 || player_pos >= MAX_PLAYERS)
    {
        exception_wrappers::invalid_arg_wrapper("Invalid player position");
    }
    if (player_cards.find(player_pos) != player_cards.end())
    {
        exception_wrappers::invalid_arg_wrapper("Player cards already set");
    }
    player_cards[player_pos] = cards;
}

void Round::clear_round()
{
    this->game_type = NO_LEWA;
    this->first_player = NONE_POS;
    this->player_cards.clear();
}

void Round::print_round()
{
    static const std::string game_type_str[] = {"NO_LEWA", "NO_HEART", "NO_QUEEN", "NO_MISTER", "NO_KING_HEART", "NO_SEVEN_AND_LAST", "BANDIT"};
    static const std::string player_pos_str[] = {"N", "E", "S", "W"};

    std::cout << "Game type: " << game_type_str[this->game_type] << "\n";
    std::cout << "First player: " << player_pos_str[this->first_player] << "\n";
    for (auto player : this->player_cards)
    {
        std::cout << "Player: " << player_pos_str[player.first] << "\n";
        player.second.print_deck();
    }
}