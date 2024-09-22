#pragma once
#include "multiemu/board.hpp"
#include <vector>

namespace BoardRegistry {
std::vector<BoardInfo *> get_board_infos();
std::unique_ptr<Board> create_board(const std::string &name);
} // namespace BoardRegistry
