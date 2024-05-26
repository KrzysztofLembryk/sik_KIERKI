#ifndef ENUM_TYPES_H
#define ENUM_TYPES_H

#include <cstdint>

enum GameType : uint8_t
{
    NO_LEWA = 1,
    NO_HEART,
    NO_QUEEN,
    NO_MISTER,
    NO_KING_HEART,
    NO_SEVEN_AND_LAST,
    BANDIT
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