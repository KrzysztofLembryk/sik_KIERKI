#include "card_classes.h"
#include <iostream>
#include "read_file.h"

int main()
{
    auto res = fHandler::read_rounds_from_file("test.txt");

    for (auto round : res)
    {
        round.print_round();
    }

    return 0;
}


