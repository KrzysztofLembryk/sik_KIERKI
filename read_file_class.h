#ifndef READ_FILE_CLASS_H
#define READ_FILE_CLASS_H

#include <fstream>
#include "game_classes.h"


class ReadFile
{

private:
    std::string file_name;
    std::ifstream file;
    std::vector<game::Round> rounds;
};

#endif