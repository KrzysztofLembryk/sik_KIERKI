#ifndef PRETTY_PRINT_PACKETS_H
#define PRETTY_PRINT_PACKETS_H

#include "player_class.h"
#include "enum_types.h"
#include <memory>
#include <semaphore>

namespace pretty_packets
{
    void pretty_print_DEAL(GameType game_type, PlayerPosition first_player_position, cardCls::DeckOfCards &deck_of_cards, std::shared_ptr<std::binary_semaphore> sem_print);

    void pretty_print_BUSY(std::vector<PlayerPosition> &taken_positions,
                           std::shared_ptr<std::binary_semaphore> sem_print);

    void pretty_print_TRICK(cardCls::Lewa &lewa,
                            std::shared_ptr<Player> player_sp,
                            std::shared_ptr<std::binary_semaphore> sem_print);

    void pretty_print_WRONG(uint8_t lewa_id,
                            std::shared_ptr<std::binary_semaphore> sem_print);

    void pretty_print_TAKEN(cardCls::Lewa &lewa,
                            PlayerPosition &player_who_took_lewa,
                            std::shared_ptr<std::binary_semaphore> sem_print);

    void pretty_print_SCORE(std::map<PlayerPosition, uint8_t> &scores,
                            std::shared_ptr<std::binary_semaphore> sem_print);

    void pretty_print_TOTAL(std::map<PlayerPosition, uint32_t> &total_scores,
                            std::shared_ptr<std::binary_semaphore> sem_print);
}

#endif // PRETTY_PRINT_PACKETS_H