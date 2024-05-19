#ifndef GAME_CLASSES_H
#define GAME_CLASSES_H

#include "card_structs.h"

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

namespace game
{
    class PointCounter
    {
        public:
        PointCounter() = delete;
        PointCounter(GameType game_type) : game_type(game_type) {}
        ~PointCounter() = default;

        uint8_t count_points(deck::Lewa &lewa);

    private:
        GameType game_type;
    };

    class CardCounter
    {
    };
} // namespace game

#endif // GAME_CLASSES_H