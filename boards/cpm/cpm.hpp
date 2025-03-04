#pragma once
#include "multiemu/board.hpp"
#include "z80.hpp"
#include <array>
#include <cstdint>

class CPMBoard : public MultiEmu::Board {
public:
  virtual void setup(const argparse::ArgumentParser &args) override;
  virtual int run(int cycles) override;

  Z80 cpu;
  bool running = true;
  std::array<uint8_t, 0x10000> memory;
};
