#pragma once
#include "argparse/argparse.hpp"
#include "multiemu/board.hpp"
#include <vector>

namespace BoardRegistry {
std::vector<BoardInfo *> get_board_infos();
BoardInfo * get_board_info(const std::string &name);
} // namespace BoardRegistry
