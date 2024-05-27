#ifndef ENUM_TYPES_H
#define ENUM_TYPES_H

#include <cstdint>

enum GameType : char
{
    NO_LEWA = '1',
    NO_HEART = '2',
    NO_QUEEN = '3',
    NO_MISTER = '4',
    NO_KING_HEART = '5',
    NO_SEVEN_AND_LAST = '6',
    BANDIT = '7'
};

/**
 * @brief Enum for player position - it's uint8_t, not char because we want to 
 * easily determine next player by incrementing the value modulo 4.
*/
enum PlayerPosition : uint8_t
{
    N = 0,
    E,
    S,
    W,
    NONE_POS
};

/**
 * @brief FigureCardValue has uint8_t type
*/
enum FigureCardValue : uint8_t
{
    J = 11,
    Q,
    K,
    A
};

/**
 * @brief Suit has char type
*/
enum Suit : char
{
    HEARTS = 'H',
    DIAMONDS = 'D',
    CLUBS = 'C',
    SPADES = 'S'
};

#endif // ENUM_TYPES_H`