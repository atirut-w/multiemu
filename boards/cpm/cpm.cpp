namespace Wrapped {
extern "C" {
#include <Z80.h>
}
} // namespace Wrapped

#include "cpm.hpp"
#include <cstddef>
#include <iostream>
#include <memory>

using namespace std;

unique_ptr<Board> create_cpm() { return make_unique<CPMBoard>(); }

CPMBoard::CPMBoard() {
  Wrapped::Z80 *real = static_cast<Wrapped::Z80 *>(cpu.real_cpu);
  real->pc.uint16_value = 0x100;
  real->sp.uint16_value = 0xffff;

  cpu.set_trap(0xc9, [this, real](size_t address) {
    if (real->pc.uint16_value + 2 == 0xffff)
    {
      running = false;
    }
    return 0;
  });

  cpu.fetch_opcode = [this, real](size_t address) {
    if (address == 5) {
      // TODO: Process system calls
      cout << "BDOS call" << endl;
      return 0xc9;
    }
    return static_cast<int>(memory[address]);
  };

  cpu.read = [this](size_t address) { return memory[address]; };
  cpu.write = [this](size_t address, uint8_t value) {
    memory[address] = value;
  };
}

int CPMBoard::step() {
  if (running) {
    return cpu.step();
  } else {
    return 0;
  }
}

REGISTER_BOARD(cpm, create_cpm)
