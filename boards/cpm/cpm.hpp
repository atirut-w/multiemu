#pragma once
#include "multiemu/board.hpp"
#include <array>
#include <cstdint>

class CPMBoard : public Board {
  void *cpu;
  bool running = true;

  virtual int run(int cycles) override;

public:
  CPMBoard();
  ~CPMBoard();
  
  std::array<uint8_t, 0x10000> memory;
};
