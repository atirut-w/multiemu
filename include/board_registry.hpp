#pragma once
#include "argparse/argparse.hpp"
#include "multiemu/board.hpp"
#include <vector>

namespace BoardRegistry {
std::vector<BoardInfo *> get_board_infos();
std::unique_ptr<Board> create_board(const std::string &name, const argparse::ArgumentParser &args);
} // namespace BoardRegistry
