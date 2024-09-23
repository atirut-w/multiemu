#include "cpm.hpp"
#include <cstddef>
#include <iostream>
#include <memory>

using namespace std;

unique_ptr<Board> create_cpm() {
  return make_unique<CPMBoard>();
}

CPMBoard::CPMBoard() {
  cpu.read = [this](size_t address) {
    return memory[address];
  };
  cpu.write = [this](size_t address, uint8_t value) {
    memory[address] = value;
  };
}

int CPMBoard::step() {
  if (running)
  {
    return cpu.step();
  }
  else
  {
    cout << "Board stopped" << endl;
    return 0;
  }
}

REGISTER_BOARD(cpm, create_cpm)
