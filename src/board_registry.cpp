#include "multiemu/board.hpp"
#include <board_registry.hpp>

using namespace std;
using namespace argparse;

extern BoardInfo __start_boards;
extern BoardInfo __stop_boards;

vector<BoardInfo *> BoardRegistry::get_board_infos() {
  const auto start = &__start_boards;
  const auto stop = &__stop_boards;

  vector<BoardInfo *> board_infos;
  for (auto *board = start; board != stop; board++) {
    board_infos.push_back(board);
  }

  return board_infos;
}

BoardInfo *BoardRegistry::get_board_info(const string &name) {
  for (auto *board : get_board_infos()) {
    if (board->name == name) {
      return board;
    }
  }

  return nullptr;
}
