#pragma once
#include <board.hpp>
#include <memory>
#include <vector>
#include <string>

namespace BoardRegistry
{
    std::vector<const BoardMeta *> get_board_metas();
    std::unique_ptr<Board> get_board(const std::string &name);
}
