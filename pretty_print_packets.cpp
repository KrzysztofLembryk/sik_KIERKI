#include "pretty_print_packets.h"

#include <iostream>
#include "common.h"
#include <vector>

using std::cout;

void pretty_packets::pretty_print_DEAL(GameType game_type, PlayerPosition first_player_position, cardCls::DeckOfCards &deck_of_cards)
{
   cout << "New deal " << gameType_to_char(game_type) << ": staring place " << playerPos_to_char(first_player_position) << ", your cards: ";

   deck_of_cards.print_deck();
   cout << ".\n";
   fflush(stdout);
}

void pretty_packets::pretty_print_BUSY(std::vector<PlayerPosition> &taken_positions)
{
   cout << "Place busy, list of busy places received: ";
   size_t i = 0;
   for (auto &pos : taken_positions)
   {
      cout << playerPos_to_char(pos);
      if (i < taken_positions.size() - 1)
         cout << ", ";
      i++;
   }
   cout << ".\n";
   fflush(stdout);
}

void pretty_packets::pretty_print_TRICK(cardCls::Lewa &lewa, 
                                        std::shared_ptr<Player> player_sp)
{
   cout << "Trick: (" << (unsigned)lewa.get_lewa_id() << ") ";
   lewa.print();
   cout << "\n";
   cout << "Available: ";
   player_sp->print_available_cards(); 
   cout << "\n";
   fflush(stdout);
}

void pretty_packets::pretty_print_WRONG(uint8_t lewa_id)
{
   cout << "Wrong message received in trick " << (unsigned)lewa_id << ".\n";
   fflush(stdout);
}   

void pretty_packets::pretty_print_TAKEN(cardCls::Lewa &lewa, 
                                        PlayerPosition &player_who_took_lewa)
{
    cout << "A trick " << (unsigned)lewa.get_lewa_id() << " is taken by " <<
    playerPos_to_char(player_who_took_lewa) << ", cards ";
    lewa.print();
    cout << ".\n";
   fflush(stdout);
}

void pretty_packets::pretty_print_SCORE(std::map<PlayerPosition, uint8_t> &scores)
{
   cout << "The scores are:\n";
   for (auto &score : scores)
   {
      cout << playerPos_to_char(score.first) << " | " << (unsigned)score.second << "\n";
   }
   fflush(stdout);
}   

void pretty_packets::pretty_print_TOTAL(std::map<PlayerPosition, uint32_t> &total_scores)
{
   cout << "The total scores are:\n";
   for (auto &score : total_scores)
   {
      cout << playerPos_to_char(score.first) << " | " << score.second << "\n";
   }
   fflush(stdout);
}