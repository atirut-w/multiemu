#include "cpm.hpp"
#include <Z80.h>
#include <cstdint>
#include <cstring>
#include <memory>

using namespace std;

unique_ptr<Board> create_cpm() { return make_unique<CPMBoard>(); }

CPMBoard *get_self(void *self) { return static_cast<CPMBoard *>(self); }

Z80 *get_cpu(void *cpu) { return static_cast<Z80 *>(cpu); }

uint8_t fetch_opcode(void *ctx, uint16_t address) {
  // TODO: Trap BDOS calls
  return static_cast<CPMBoard *>(ctx)->memory[address];
}

uint8_t read(void *ctx, uint16_t address) {
  return static_cast<CPMBoard *>(ctx)->memory[address];
}

void write(void *ctx, uint16_t address, uint8_t value) {
  static_cast<CPMBoard *>(ctx)->memory[address] = value;
}

uint8_t in(void *ctx, uint16_t port) { return 0; }

void out(void *ctx, uint16_t port, uint8_t value) {}

CPMBoard::CPMBoard() {
  Z80 *cpu = new Z80();
  cpu->context = this;

  cpu->fetch_opcode = fetch_opcode;
  cpu->fetch = read;
  cpu->read = read;
  cpu->write = write;
  cpu->in = in;
  cpu->out = out;

  this->cpu = cpu;
}

CPMBoard::~CPMBoard() { delete static_cast<Z80 *>(cpu); }

int CPMBoard::run(int cycles) {
  if (running) {
    return z80_run(get_cpu(cpu), cycles);
  } else {
    return 0;
  }
}

REGISTER_BOARD(cpm, create_cpm)
