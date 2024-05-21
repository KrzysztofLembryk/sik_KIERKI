#include "card_classes.h"
#include <iostream>
#include "read_file.h"

int main()
{
    auto res = fHandler::read_rounds_from_file("testii.txt");
    res[0].get_player_cards(N).print();
    return 0;
}


