#include "cpm.hpp"
#include <memory>

using namespace std;

unique_ptr<Board> create_cpm() {
  return make_unique<CPMBoard>();
}

int CPMBoard::step() {
  return 0;
}

REGISTER_BOARD(cpm, create_cpm)
