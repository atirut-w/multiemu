#include "cpm.hpp"
#include "argparse/argparse.hpp"
#include <Z80.h>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <memory>

using namespace std;
using namespace argparse;

unique_ptr<Board> create_cpm(const ArgumentParser &args) {
  auto board = make_unique<CPMBoard>();
  ifstream rom(args.get<filesystem::path>("program"), ios::binary);
  if (!rom) {
    throw runtime_error("Cannot open ROM file");
  }
  rom.read(reinterpret_cast<char *>(&board->memory[0x100]), 0x10000 - 0x100);

  return board;
}

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
  cpu->sp.uint16_value = 0;
  cpu->pc.uint16_value = 0x100;
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
