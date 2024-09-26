#pragma once
#include "fantacom/mmu.hpp"
#include "multiemu/board.hpp"
#include "z80.hpp"
#include <cstdint>

class FantacomBoard : public Board {
public:
  FantacomBoard();

  Z80 cpu;
  std::array<uint8_t, 0x4000> rom;
  MMU mmu;

  virtual int run(int cycles) override;
  static FantacomBoard *get_self(void *ctx);
  static uint8_t read(void *ctx, uint16_t address);
  static void write(void *ctx, uint16_t address, uint8_t value);
};
