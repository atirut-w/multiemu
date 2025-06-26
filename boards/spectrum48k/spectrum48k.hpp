#pragma once
#include "cpu/z80.hpp"
#include "multiemu/board.hpp"
#include "multiemu/bus.hpp"
#include "multiemu/cpu.hpp"
#include <array>
#include <cstdint>
#include <memory>
#include <vector>

namespace MultiEmu {

class Spectrum48K : public Board {
public:
  Spectrum48K();
  ~Spectrum48K() override = default;

  // Implement Board interface
  void setup(const argparse::ArgumentParser &args) override;
  int run(int cycles) override;
  void draw() override;
  std::vector<BusInfo> get_buses() const override;
  CPU *getCPU() override { return &z80; }

private:
  // Hardware components
  Z80 z80;
  Bus memory_bus;
  Bus io_bus;

  // Memory
  std::vector<uint8_t> rom;           // 16KB ROM
  std::array<uint8_t, 48 * 1024> ram; // 48KB RAM

  // Keyboard state - 8 half-rows with 5 keys each
  // False = key pressed, True = key released
  std::array<std::array<bool, 5>, 8> keyboard;

  // Interrupt timing
  int interrupt_cycles;                              // Cycles elapsed since last interrupt
  static constexpr int CYCLES_PER_INTERRUPT = 69888; // ~50Hz (3.5MHz / 50Hz)

  // FLASH attribute state
  bool flash_state = false;                          // Current flash state (false = normal, true = inverted)
  int flash_counter = 0;                             // Counter for flash timing

  // Memory access
  uint8_t read_memory(uint16_t address);
  void write_memory(uint16_t address, uint8_t value);

  // I/O port access
  uint8_t read_port(uint16_t port);
  void write_port(uint16_t port, uint8_t value);

  // Handle keyboard input
  void handle_keypress(int row, int col, bool pressed);
};

} // namespace MultiEmu
