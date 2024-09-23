#include <cstddef>
#include <cstdint>
#include <cstring>

using namespace std;

namespace Wrapped {
  extern "C" {
    #include <Z80.h>
  }
}
#include "z80/z80.hpp"

Wrapped::Z80 *get_cpu(void *cpu) {
  return static_cast<Wrapped::Z80 *>(cpu);
}

Z80 *get_self(void *self) {
  return static_cast<Z80 *>(self);
}

uint8_t fetch_opw(void *ctx, uint16_t address) {
  uint8_t opcode = get_self(ctx)->read(address);
  if (get_self(ctx)->traps.count(opcode))
    return get_self(ctx)->traps[opcode](address);
  return get_self(ctx)->fetch_opcode(address);
}

uint8_t readw(void *ctx, uint16_t address) {
  return get_self(ctx)->read(address);
}

void writew(void *ctx, uint16_t address, uint8_t value) {
  get_self(ctx)->write(address, value);
}

uint8_t inw(void *ctx, uint16_t port) {
  return get_self(ctx)->in(port);
}

void outw(void *ctx, uint16_t port, uint8_t value) {
  get_self(ctx)->out(port, value);
}

Z80::Z80() {
  Wrapped::Z80 *cpu = new Wrapped::Z80();
  cpu->context = this;

  cpu->fetch_opcode = fetch_opw;
  cpu->fetch = readw;
  cpu->read = readw;
  cpu->write = writew;
  cpu->in = inw;
  cpu->out = outw;

  real_cpu = cpu;
}

Z80::~Z80() {
  delete static_cast<Wrapped::Z80 *>(real_cpu);
}

int Z80::run(int cycles) {
  return Wrapped::z80_run(get_cpu(real_cpu), cycles);
}

void Z80::set_trap(size_t opcode, function<size_t(size_t)> handler) {
  traps[opcode] = handler;
}
