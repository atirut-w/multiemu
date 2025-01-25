#pragma once
#include "argparse/argparse.hpp"
#include "multiemu/board.hpp"
#include <vector>

// namespace MultiEmu::BoardRegistry {
// std::vector<BoardInfo *> get_board_infos();
// BoardInfo * get_board_info(const std::string &name);
// } // namespace BoardRegistry

namespace MultiEmu {

class BoardRegistry {
  static std::vector<Board *> boards;

public:
  static const std::vector<Board *> &get_boards();
  static Board *get_board(const std::string &name);

  template <class BoardType> class Register {
  public:
    Register() {
      static BoardType board;
      boards.push_back(&board);
    }
  };
};

} // namespace MultiEmu
