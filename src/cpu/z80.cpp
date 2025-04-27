#include "cpu/z80.hpp"
#include <iostream>
#include <stdexcept>

// Include the Chips library Z80 header
#define CHIPS_IMPL
#include "chips/z80.h"

namespace MultiEmu {

// Define the implementation struct
struct Z80::Impl {
  z80_t cpu;
  uint64_t pins;

  Impl(Z80 *parent) {
    // Initialize the Z80 CPU
    pins = z80_init(&cpu);
    
    // Setup memory callbacks indirectly by passing pins to tick function
  }
  
  // Helper function to handle memory and IO access
  uint64_t tick(uint64_t pins) {
    // Check if memory read/write is requested
    if (pins & Z80_MREQ) {
      uint16_t addr = Z80_GET_ADDR(pins);
      if (pins & Z80_RD) {
        // Memory read
        uint8_t data = memoryRead(addr);
        Z80_SET_DATA(pins, data);
      } else if (pins & Z80_WR) {
        // Memory write
        uint8_t data = Z80_GET_DATA(pins);
        memoryWrite(addr, data);
      }
    } else if (pins & Z80_IORQ) {
      // Handle IO operations
      uint16_t port = Z80_GET_ADDR(pins);
      if (pins & Z80_RD) {
        // IO read
        uint8_t data = ioRead(port);
        Z80_SET_DATA(pins, data);
      } else if (pins & Z80_WR) {
        // IO write
        uint8_t data = Z80_GET_DATA(pins);
        ioWrite(port, data);
      }
    }
    return pins;
  }
  
  // Memory access functions using the parent's callbacks
  uint8_t memoryRead(uint16_t addr) {
    if (parent && parent->read) {
      return parent->read(addr);
    }
    return 0xFF;
  }
  
  void memoryWrite(uint16_t addr, uint8_t value) {
    if (parent && parent->write) {
      parent->write(addr, value);
    }
  }
  
  // IO access functions using the parent's callbacks
  uint8_t ioRead(uint16_t port) {
    if (parent && parent->in) {
      return parent->in(port);
    }
    return 0xFF;
  }
  
  void ioWrite(uint16_t port, uint8_t value) {
    if (parent && parent->out) {
      parent->out(port, value);
    }
  }
  
  Z80* parent = nullptr;
};

// Constructor - initialize implementation
Z80::Z80() : pImpl(std::make_unique<Impl>(this)) {
  pImpl->parent = this;  // Set the parent pointer
}

// Destructor - needed for unique_ptr with incomplete type
Z80::~Z80() = default;

// Move constructor
Z80::Z80(Z80 &&) noexcept = default;

// Move assignment
Z80 &Z80::operator=(Z80 &&) noexcept = default;

// Z80-specific debugger capabilities
DebuggerCapabilities Z80::getDebuggerCapabilities() const {
  DebuggerCapabilities caps;
  caps.hasStatusRegister = true;
  caps.statusRegisterName = "F";
  return caps;
}

// Z80 register information
std::vector<RegisterInfo> Z80::getRegisterInfo() const {
  return {// 8-bit registers
          {"A", 8, "hex", "Accumulator"},
          {"F", 8, "hex", "Flags"},
          {"B", 8, "hex", "General Purpose"},
          {"C", 8, "hex", "General Purpose"},
          {"D", 8, "hex", "General Purpose"},
          {"E", 8, "hex", "General Purpose"},
          {"H", 8, "hex", "General Purpose"},
          {"L", 8, "hex", "General Purpose"},
          {"I", 8, "hex", "Interrupt Vector"},
          {"R", 8, "hex", "Memory Refresh"},

          // 16-bit registers
          {"AF", 16, "hex", "Accumulator & Flags"},
          {"BC", 16, "hex", "General Purpose Pair"},
          {"DE", 16, "hex", "General Purpose Pair"},
          {"HL", 16, "hex", "General Purpose Pair"},
          {"IX", 16, "hex", "Index Register X"},
          {"IY", 16, "hex", "Index Register Y"},
          {"SP", 16, "hex", "Stack Pointer"},
          {"PC", 16, "hex", "Program Counter"},

          // CPU state
          {"IM", 2, "dec", "Interrupt Mode"},
          {"IFF", 8, "hex", "Interrupt Flip-Flop"}};
}

// Z80 register grouping
std::vector<RegisterGroup> Z80::getRegisterGroups() const {
  return {{"8-bit", {"A", "F", "B", "C", "D", "E", "H", "L", "I", "R"}},
          {"16-bit", {"PC", "SP", "IX", "IY"}},
          {"Pairs", {"AF", "BC", "DE", "HL"}},
          {"Status", {"IM", "IFF"}}};
}

// Z80 flag definitions
std::vector<FlagDefinition> Z80::getFlagDefinitions() const {
  return {{"Sign", 'S', 7, "Set if result is negative"},
          {"Zero", 'Z', 6, "Set if result is zero"},
          {"5", '5', 5, "Bit 5 (undocumented)"},
          {"Half-Carry", 'H', 4, "Set if carry from bit 3"},
          {"3", '3', 3, "Bit 3 (undocumented)"},
          {"Parity/Overflow", 'P', 2, "Parity of result / Overflow occurred"},
          {"Subtract", 'N', 1, "Set if last operation was subtraction"},
          {"Carry", 'C', 0, "Set if result overflowed"}};
}

// Core execution methods
int Z80::execute(int cycles) {
  int executed = 0;
  
  // Run for the requested number of cycles
  while (executed < cycles) {
    // Tick the CPU
    pImpl->pins = z80_tick(&pImpl->cpu, pImpl->pins);
    
    // Process memory and IO requests
    pImpl->pins = pImpl->tick(pImpl->pins);
    
    executed++;
  }
  
  return executed;
}

void Z80::reset() {
  // Reset the CPU
  pImpl->pins = z80_reset(&pImpl->cpu);
}

// Register access
uint64_t Z80::getRegister(const std::string &name) {
  const z80_t& cpu = pImpl->cpu;
  
  if (name == "A") return cpu.a;
  if (name == "F") return cpu.f;
  if (name == "B") return cpu.b;
  if (name == "C") return cpu.c;
  if (name == "D") return cpu.d;
  if (name == "E") return cpu.e;
  if (name == "H") return cpu.h;
  if (name == "L") return cpu.l;
  if (name == "AF") return cpu.af;
  if (name == "BC") return cpu.bc;
  if (name == "DE") return cpu.de;
  if (name == "HL") return cpu.hl;
  if (name == "IX") return cpu.ix;
  if (name == "IY") return cpu.iy;
  if (name == "SP") return cpu.sp;
  if (name == "PC") return cpu.pc;
  if (name == "I") return cpu.i;
  if (name == "R") return cpu.r;
  if (name == "IFF") return (cpu.iff1 ? 1 : 0) | (cpu.iff2 ? 2 : 0);
  if (name == "IM") return cpu.im;
  
  return 0;
}

bool Z80::setRegister(const std::string &name, uint64_t value) {
  z80_t& cpu = pImpl->cpu;
  
  if (name == "A") { cpu.a = value & 0xFF; return true; }
  if (name == "F") { cpu.f = value & 0xFF; return true; }
  if (name == "B") { cpu.b = value & 0xFF; return true; }
  if (name == "C") { cpu.c = value & 0xFF; return true; }
  if (name == "D") { cpu.d = value & 0xFF; return true; }
  if (name == "E") { cpu.e = value & 0xFF; return true; }
  if (name == "H") { cpu.h = value & 0xFF; return true; }
  if (name == "L") { cpu.l = value & 0xFF; return true; }
  if (name == "AF") { 
    cpu.a = (value >> 8) & 0xFF;
    cpu.f = value & 0xFF;
    return true;
  }
  if (name == "BC") {
    cpu.b = (value >> 8) & 0xFF;
    cpu.c = value & 0xFF;
    return true;
  }
  if (name == "DE") {
    cpu.d = (value >> 8) & 0xFF;
    cpu.e = value & 0xFF;
    return true;
  }
  if (name == "HL") {
    cpu.h = (value >> 8) & 0xFF;
    cpu.l = value & 0xFF;
    return true;
  }
  if (name == "IX") { cpu.ix = value & 0xFFFF; return true; }
  if (name == "IY") { cpu.iy = value & 0xFFFF; return true; }
  if (name == "SP") { cpu.sp = value & 0xFFFF; return true; }
  if (name == "PC") { 
    // Use prefetch to set PC properly
    pImpl->pins = z80_prefetch(&pImpl->cpu, value & 0xFFFF);
    return true;
  }
  if (name == "I") { cpu.i = value & 0xFF; return true; }
  if (name == "R") { cpu.r = value & 0xFF; return true; }
  if (name == "IFF") {
    cpu.iff1 = (value & 1) != 0;
    cpu.iff2 = (value & 2) != 0;
    return true;
  }
  if (name == "IM") { cpu.im = value & 0x03; return true; }
  
  return false;
}

std::map<std::string, uint64_t> Z80::getAllRegisters() {
  std::map<std::string, uint64_t> registers;
  const z80_t& cpu = pImpl->cpu;
  
  // 8-bit registers
  registers["A"] = cpu.a;
  registers["F"] = cpu.f;
  registers["B"] = cpu.b;
  registers["C"] = cpu.c;
  registers["D"] = cpu.d;
  registers["E"] = cpu.e;
  registers["H"] = cpu.h;
  registers["L"] = cpu.l;
  registers["I"] = cpu.i;
  registers["R"] = cpu.r;
  
  // 16-bit registers
  registers["AF"] = cpu.af;
  registers["BC"] = cpu.bc;
  registers["DE"] = cpu.de;
  registers["HL"] = cpu.hl;
  registers["IX"] = cpu.ix;
  registers["IY"] = cpu.iy;
  registers["SP"] = cpu.sp;
  registers["PC"] = cpu.pc;
  
  // CPU state
  registers["IM"] = cpu.im;
  registers["IFF"] = (cpu.iff1 ? 1 : 0) | (cpu.iff2 ? 2 : 0);
  
  return registers;
}

// Program counter access
size_t Z80::getProgramCounter() const {
  return pImpl->cpu.pc;
}

// Interrupt interface implementation
void Z80::requestInterrupt(uint32_t vector, bool nmi) {
  if (nmi) {
    // Set the NMI pin
    pImpl->pins |= Z80_NMI;
  } else {
    // Set the INT pin and prepare vector
    pImpl->pins |= Z80_INT;
    // The vector will be placed on the data bus during interrupt acknowledge
  }
}

bool Z80::areInterruptsEnabled() const {
  // IFF1 determines if interrupts are enabled
  return pImpl->cpu.iff1;
}

} // namespace MultiEmu
