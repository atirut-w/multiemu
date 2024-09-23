#pragma once
#include "multiemu/board.hpp"
#include "z80/z80.hpp"
#include <array>
#include <cstdint>

class CPMBoard : public Board {
  Z80 cpu;
  std::array<uint8_t, 0x10000> memory;
  bool running = true;

  virtual int step() override;

public:
  CPMBoard();
};
