#ifndef READ_FILE_CLASS_H
#define READ_FILE_CLASS_H

#include <fstream>
#include <vector>
#include "enum_types.h"

class Round
{

private:
    GameType game_type;
    PlayerPosition first_player;
};

class ReadFile
{

private:
    std::string file_name;
    std::ifstream file;
    std::vector<Round> rounds;
};

#endif