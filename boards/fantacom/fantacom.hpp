#pragma once
#include "argparse/argparse.hpp"
#include "fantacom/graphics.hpp"
#include "fantacom/mmu.hpp"
#include "multiemu/board.hpp"
#include "multiemu/bus.hpp"
#include "multiemu/ram.hpp"
#include "multiemu/rom.hpp"
#include "z80.hpp"
#include <cstdint>
#include <vector>

class FantacomBoard : public MultiEmu::Board {
public:
  FantacomBoard(const argparse::ArgumentParser &args);

  Z80 cpu;
  Graphics gfx;
  MultiEmu::ROM rom;
  MultiEmu::RAM ram;
  MMU mmu;

  MultiEmu::Bus<uint8_t> rambus;
  MultiEmu::Bus<uint8_t> io;

  virtual int run(int cycles) override;
  virtual void draw() override;
  
  static FantacomBoard *get_self(void *ctx);
  static uint8_t read(void *ctx, uint16_t address);
  static void write(void *ctx, uint16_t address, uint8_t value);
  static uint8_t in(void *ctx, uint16_t address);
  static void out(void *ctx, uint16_t address, uint8_t value);
};
