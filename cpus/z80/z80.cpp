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

Z80::Z80() {
  Wrapped::Z80 *cpu = new Wrapped::Z80();
  real_cpu = cpu;
}

Z80::~Z80() {
  delete static_cast<Wrapped::Z80 *>(real_cpu);
}

int Z80::step() {
  return Wrapped::z80_run(get_cpu(real_cpu), 1);
}
