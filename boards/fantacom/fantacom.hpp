#pragma once
#include "fantacom/mmu.hpp"
#include "multiemu/board.hpp"
#include "multiemu/bus.hpp"
#include "z80.hpp"
#include <cstdint>
#include <vector>

class FantacomBoard : public Board {
public:
  FantacomBoard();

  Z80 cpu;
  std::array<uint8_t, 0x4000> rom;
  std::vector<uint8_t> ram;
  MMU mmu;
  Bus<uint8_t> io;

  virtual int run(int cycles) override;
  static FantacomBoard *get_self(void *ctx);
  static uint8_t read(void *ctx, uint16_t address);
  static void write(void *ctx, uint16_t address, uint8_t value);
  static uint8_t in(void *ctx, uint16_t address);
  static void out(void *ctx, uint16_t address, uint8_t value);
};
