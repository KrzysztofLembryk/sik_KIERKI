#ifndef GAME_CLASSES_H
#define GAME_CLASSES_H

#include "card_classes.h"

namespace gameCls
{
    /**
     * @brief PointCounter class is used to count points for given Lewa
     * regarding game type.
     */
    class PointCounter
    {
    public:
        PointCounter() = delete;
        PointCounter(GameType game_type) : game_type(game_type) {}
        ~PointCounter() = default;

        uint8_t count_points(cardCls::Lewa &lewa);
        void set_game_type(GameType game_type);

    private:
        GameType game_type;
    };

    /**
     * @brief CardCounter class is used to count cards played in current round.
     * It stores number of lewas, hearts, queens, misters, king of hearts.
     * It counts all relevant cards, but depending on game type in
     * has_game_ended() method it checks if game has ended using diffetrent
     * criterias for each game type using different variables.
     */
    class CardCounter
    {
    public:
        CardCounter() = delete;
        CardCounter(GameType game_type) : game_type(game_type), nbr_of_lewas_played(0), nbr_of_hearts_played(0), nbr_of_queens_played(0), nbr_of_misters_played(0), king_of_hearts_played(false), seven_and_last_played(false) {}
        ~CardCounter() = default;

        void count_cards(cardCls::Lewa &lewa);
        bool has_game_ended();

    private:
        GameType game_type;
        uint8_t nbr_of_lewas_played;
        uint8_t nbr_of_hearts_played;
        uint8_t nbr_of_queens_played;
        uint8_t nbr_of_misters_played;
        bool king_of_hearts_played;
        bool seven_and_last_played;
    };

    class Round
    {
    public:
        Round() = default;
        Round(GameType game_type, PlayerPosition first_player) : game_type(game_type), first_player(first_player) {}
        ~Round() = default;

        GameType get_game_type() const;
        PlayerPosition get_first_player() const;
        cardCls::DeckOfCards &get_player_cards(PlayerPosition player_pos);
        void set_game_type(GameType game_type);
        void set_first_player(PlayerPosition first_player);
        void set_player_cards(PlayerPosition player_pos,
                              cardCls::DeckOfCards &cards);
        void clear_round();
        void print_round();

    private:
        GameType game_type;
        PlayerPosition first_player;
        std::map<PlayerPosition, cardCls::DeckOfCards> player_cards;
    };

} // namespace game

#endif // GAME_CLASSES_H