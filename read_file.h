#ifndef READ_FILE_H
#define READ_FILE_H

#include <fstream>
#include <vector>
#include "game_classes.h"

namespace fHandler
{
    std::vector<game::Round> read_rounds_from_file(const std::string &file_path);
}

#endif