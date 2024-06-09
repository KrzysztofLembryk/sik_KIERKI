#ifndef PRETTY_PRINT_PACKETS_H
#define PRETTY_PRINT_PACKETS_H

#include "player_class.h"
#include "enum_types.h"

namespace pretty_packets
{
    void pretty_print_DEAL(GameType game_type, PlayerPosition first_player_position,cardCls::DeckOfCards &deck_of_cards);
    void pretty_print_BUSY(std::vector<PlayerPosition> &taken_positions);
    void pretty_print_TRICK(cardCls::Lewa &lewa);
    void pretty_print_WRONG(uint8_t lewa_id);
    void pretty_print_TAKEN(cardCls::Lewa &lewa, 
                            PlayerPosition &player_who_took_lewa);
    void pretty_print_SCORE(std::map<PlayerPosition, uint8_t> &scores);
    void pretty_print_TOTAL(std::map<PlayerPosition, uint32_t> &total_scores);
}

#endif // PRETTY_PRINT_PACKETS_H