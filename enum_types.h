#ifndef ENUM_TYPES_H
#define ENUM_TYPES_H

enum GameType
{
    NO_LEWA = 1,
    NO_HEART,
    NO_QUEEN,
    NO_MISTER,
    NO_KING_HEART,
    NO_SEVEN_AND_LAST,
    BANDIT
};

enum PlayerPosition
{
    N = 0,
    E,
    S,
    W,
    NONE_POS
};

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

#endif // ENUM_TYPES_H`