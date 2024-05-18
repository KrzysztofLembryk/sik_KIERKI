#include "card_structs.h"
#include <iostream>

int main()
{
    deck::Card h10 = {HEARTS, 10};
    deck::Card dJ = {DIAMONDS, J};
    deck::AllCards all_cards;

    all_cards.was_card_played_map[deck::CardClassWrapper(h10)] = true;
    all_cards.was_card_played_map[deck::CardClassWrapper(dJ)] = false;

    deck::Card h10_new = {HEARTS, 10};
    deck::Card dJ_new = {DIAMONDS, J};

    if (all_cards.was_card_played_map[deck::CardClassWrapper(h10_new)])
    {
        std::cout << "h10_new was played" << "\n";
    }
    else
    {
        std::cout << "h10_new was not played" << "\n";
    }

    return 0;
}


