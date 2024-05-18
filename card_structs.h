#ifndef CARD_STRUCTS_H
#define CARD_STRUCTS_H

#include <cstdint>

enum FigureCardValue {
    J = 11,
    Q,
    K,
    A
};

enum Suit {
    HEARTS = 'H',
    DIAMONDS = 'D',
    CLUBS = 'C',
    SPADES = 'S'
};

typedef struct __attribute__ ((__packed__)) Card {
    Suit suit;
    uint8_t value;
} Card;


#endif // COMMUNICATION_STRUCTS_H