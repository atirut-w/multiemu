#include "fantacom/fantacom.hpp"
#include "argparse/argparse.hpp"
#include "multiemu/board.hpp"
#include <memory>

using namespace std;
using namespace argparse;

unique_ptr<Board> create_fantacom(const ArgumentParser &args) {
  auto board = make_unique<FantacomBoard>();
  return board;
}
REGISTER_BOARD(fantacom, create_fantacom, {.display = true});

int FantacomBoard::run(int cycles) {
  return 1;
}
