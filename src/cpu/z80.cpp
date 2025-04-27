#include "cpu/z80.hpp"
#include <iostream>
#include <stdexcept>
#include <z80.hpp>

namespace MultiEmu {

// Define the implementation struct
struct Z80::Impl {
  ::Z80 cpu;

  Impl(Z80 *parent) {
    // Set up callbacks, capturing the parent Z80 class pointer
    cpu.setupCallback([parent](void *, uint16_t address) { return parent->read(address); },
                      [parent](void *, uint16_t address, uint8_t value) { parent->write(address, value); },
                      [parent](void *, uint16_t address) { return parent->in(address); },
                      [parent](void *, uint16_t address, uint8_t value) { parent->out(address, value); });
  }
};

// Constructor - initialize implementation
Z80::Z80() : pImpl(std::make_unique<Impl>(this)) {}

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
  // Execute for requested cycles
  return pImpl->cpu.execute(cycles);
}

void Z80::reset() {
  // The Z80 library doesn't have a reset method, so we'll implement it manually
  // NOTE: We don't reset every registers, just the bare minimum.
  pImpl->cpu.reg.IFF = 0;       // Reset interrupt flip-flop
  pImpl->cpu.reg.interrupt = 0; // Reset interrupt mode
  pImpl->cpu.reg.PC = 0;        // Reset program counter
  pImpl->cpu.reg.I = 0;         // Reset interrupt vector
  pImpl->cpu.reg.R = 0;         // Reset memory refresh
}

// Register access helpers
namespace {
// Helper to map register names to Z80 register values
uint64_t getZ80Register(const ::Z80 &cpu, const std::string &name) {
  if (name == "A")
    return cpu.reg.pair.A;
  if (name == "F")
    return cpu.reg.pair.F;
  if (name == "B")
    return cpu.reg.pair.B;
  if (name == "C")
    return cpu.reg.pair.C;
  if (name == "D")
    return cpu.reg.pair.D;
  if (name == "E")
    return cpu.reg.pair.E;
  if (name == "H")
    return cpu.reg.pair.H;
  if (name == "L")
    return cpu.reg.pair.L;
  if (name == "AF")
    return (cpu.reg.pair.A << 8) | cpu.reg.pair.F;
  if (name == "BC")
    return (cpu.reg.pair.B << 8) | cpu.reg.pair.C;
  if (name == "DE")
    return (cpu.reg.pair.D << 8) | cpu.reg.pair.E;
  if (name == "HL")
    return (cpu.reg.pair.H << 8) | cpu.reg.pair.L;
  if (name == "IX")
    return cpu.reg.IX;
  if (name == "IY")
    return cpu.reg.IY;
  if (name == "SP")
    return cpu.reg.SP;
  if (name == "PC")
    return cpu.reg.PC;
  if (name == "I")
    return cpu.reg.I;
  if (name == "R")
    return cpu.reg.R;
  if (name == "IFF")
    return cpu.reg.IFF;
  // Extract interrupt mode from the interrupt register (bits 0-1)
  if (name == "IM")
    return cpu.reg.interrupt & 0x03;
  return 0;
}

bool setZ80Register(::Z80 &cpu, const std::string &name, uint64_t value) {
  if (name == "A") {
    cpu.reg.pair.A = value & 0xFF;
    return true;
  }
  if (name == "F") {
    cpu.reg.pair.F = value & 0xFF;
    return true;
  }
  if (name == "B") {
    cpu.reg.pair.B = value & 0xFF;
    return true;
  }
  if (name == "C") {
    cpu.reg.pair.C = value & 0xFF;
    return true;
  }
  if (name == "D") {
    cpu.reg.pair.D = value & 0xFF;
    return true;
  }
  if (name == "E") {
    cpu.reg.pair.E = value & 0xFF;
    return true;
  }
  if (name == "H") {
    cpu.reg.pair.H = value & 0xFF;
    return true;
  }
  if (name == "L") {
    cpu.reg.pair.L = value & 0xFF;
    return true;
  }
  if (name == "AF") {
    cpu.reg.pair.A = (value >> 8) & 0xFF;
    cpu.reg.pair.F = value & 0xFF;
    return true;
  }
  if (name == "BC") {
    cpu.reg.pair.B = (value >> 8) & 0xFF;
    cpu.reg.pair.C = value & 0xFF;
    return true;
  }
  if (name == "DE") {
    cpu.reg.pair.D = (value >> 8) & 0xFF;
    cpu.reg.pair.E = value & 0xFF;
    return true;
  }
  if (name == "HL") {
    cpu.reg.pair.H = (value >> 8) & 0xFF;
    cpu.reg.pair.L = value & 0xFF;
    return true;
  }
  if (name == "IX") {
    cpu.reg.IX = value & 0xFFFF;
    return true;
  }
  if (name == "IY") {
    cpu.reg.IY = value & 0xFFFF;
    return true;
  }
  if (name == "SP") {
    cpu.reg.SP = value & 0xFFFF;
    return true;
  }
  if (name == "PC") {
    cpu.reg.PC = value & 0xFFFF;
    return true;
  }
  if (name == "I") {
    cpu.reg.I = value & 0xFF;
    return true;
  }
  if (name == "R") {
    cpu.reg.R = value & 0xFF;
    return true;
  }
  if (name == "IFF") {
    cpu.reg.IFF = value & 0xFF;
    return true;
  }
  if (name == "IM") {
    // Only modify the interrupt mode bits (0-1)
    cpu.reg.interrupt = (cpu.reg.interrupt & 0xFC) | (value & 0x03);
    return true;
  }
  return false;
}
} // namespace

// Register access
uint64_t Z80::getRegister(const std::string &name) { return getZ80Register(pImpl->cpu, name); }

bool Z80::setRegister(const std::string &name, uint64_t value) { return setZ80Register(pImpl->cpu, name, value); }

std::map<std::string, uint64_t> Z80::getAllRegisters() {
  std::map<std::string, uint64_t> registers;
  const auto &cpu = pImpl->cpu;

  // 8-bit registers
  registers["A"] = cpu.reg.pair.A;
  registers["F"] = cpu.reg.pair.F;
  registers["B"] = cpu.reg.pair.B;
  registers["C"] = cpu.reg.pair.C;
  registers["D"] = cpu.reg.pair.D;
  registers["E"] = cpu.reg.pair.E;
  registers["H"] = cpu.reg.pair.H;
  registers["L"] = cpu.reg.pair.L;
  registers["I"] = cpu.reg.I;
  registers["R"] = cpu.reg.R;

  // 16-bit registers
  registers["AF"] = (cpu.reg.pair.A << 8) | cpu.reg.pair.F;
  registers["BC"] = (cpu.reg.pair.B << 8) | cpu.reg.pair.C;
  registers["DE"] = (cpu.reg.pair.D << 8) | cpu.reg.pair.E;
  registers["HL"] = (cpu.reg.pair.H << 8) | cpu.reg.pair.L;
  registers["IX"] = cpu.reg.IX;
  registers["IY"] = cpu.reg.IY;
  registers["SP"] = cpu.reg.SP;
  registers["PC"] = cpu.reg.PC;

  // CPU state
  registers["IM"] = cpu.reg.interrupt & 0x03;
  registers["IFF"] = cpu.reg.IFF;

  return registers;
}

// Program counter access
size_t Z80::getProgramCounter() { return pImpl->cpu.reg.PC; }

// Interrupt interface implementation
void Z80::requestInterrupt(uint32_t vector, bool nmi) {
  if (nmi) {
    // For Z80, NMI doesn't use vector but jumps to a fixed address (0x0066)
    // NMI cannot be masked and is always handled regardless of interrupt enable flag
    pImpl->cpu.generateNMI(0x0066);
  } else {
    // Regular maskable interrupt with vector
    // In this Z80 implementation, the vector handling depends on interrupt mode:
    // - IM 0: Uses the vector directly as an instruction operand (typically for RST instruction)
    // - IM 1: Vector is ignored, CPU always executes an RST 38h instruction
    // - IM 2: Vector is combined with I register to form an address pointer
    pImpl->cpu.generateIRQ(vector & 0xFF); // Limit vector to 8 bits for Z80
  }
}

bool Z80::areInterruptsEnabled() const {
  // IFF1 (bit 0 of IFF) determines if interrupts are enabled
  return (pImpl->cpu.reg.IFF & 0x01) != 0;
}

} // namespace MultiEmu
