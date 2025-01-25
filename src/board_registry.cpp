#include "multiemu/board.hpp"
#include <multiemu/board_registry.hpp>

using namespace std;
using namespace argparse;
using namespace MultiEmu;

vector<Board *> BoardRegistry::boards;

const vector<Board *> &BoardRegistry::get_boards() {
  return boards;
}

Board *BoardRegistry::get_board(const string &name) {
  for (auto *board : boards) {
    if (board->name == name) {
      return board;
    }
  }
  return nullptr;
}
