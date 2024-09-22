#include "cpm.hpp"
#include <iostream>
#include <memory>

using namespace std;

unique_ptr<Board> create_cpm() {
  return make_unique<CPMBoard>();
}

CPMBoard::~CPMBoard() {
    cout << "CPMBoard::~CPMBoard()" << endl;
}

REGISTER_BOARD(cpm, create_cpm)
