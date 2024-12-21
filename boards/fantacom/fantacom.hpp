#pragma once
#include "argparse/argparse.hpp"
#include "fantacom/graphics.hpp"
#include "fantacom/mmu.hpp"
#include "multiemu/board.hpp"
#include "multiemu/memory_region.hpp"
#include "multiemu/units.hpp"
#include "z80.hpp"
#include <cstdint>
#include <vector>

class FantacomBoard : public MultiEmu::Board {
public:
  FantacomBoard(const argparse::ArgumentParser &args);

  Z80 cpu;
  Graphics gfx;
  
  MultiEmu::MemoryRegionContainer phys = MultiEmu::MemoryRegionContainer(MIB);
  MemoryRegionMMU virt;
  MultiEmu::MemoryRegionContainer io = MultiEmu::MemoryRegionContainer(64 * KIB);
  MultiEmu::MemoryRegionRAM ram;
  MultiEmu::MemoryRegionROM rom = MultiEmu::MemoryRegionROM(8 * KIB);

  virtual int run(int cycles) override;
  virtual void draw() override;
  
  static FantacomBoard *get_self(void *ctx);
  static uint8_t read(void *ctx, uint16_t address);
  static void write(void *ctx, uint16_t address, uint8_t value);
  static uint8_t in(void *ctx, uint16_t address);
  static void out(void *ctx, uint16_t address, uint8_t value);
};
