#pragma once
#include <board.hpp>
#include <memory>
#include <vector>
#include <string>

namespace BoardRegistry
{
    std::vector<BoardMeta> get_boards();
    BoardMeta get_board(const std::string &name);
}
