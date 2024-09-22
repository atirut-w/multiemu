#pragma once
#include <board.hpp>
#include <memory>
#include <string>
#include <vector>

namespace BoardRegistry {
const std::vector<const BoardMeta *> get_board_metas();
std::unique_ptr<Board> get_board(const std::string &name);
} // namespace BoardRegistry
