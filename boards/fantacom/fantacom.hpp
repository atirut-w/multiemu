#pragma once
#include "argparse/argparse.hpp"
#include "fantacom/graphics.hpp"
#include "multiemu/board.hpp"
#include "multiemu/memory_region.hpp"
#include "z80.hpp"
#include <cstdint>
#include <memory>

class FantacomBoard : public MultiEmu::Board {
public:
  FantacomBoard(const argparse::ArgumentParser &args);

  Z80 cpu;
  Graphics gfx;

  std::unique_ptr<MultiEmu::MemoryRegion> phys;
  std::unique_ptr<MultiEmu::MemoryRegion> io;
  std::unique_ptr<MultiEmu::MemoryRegionRAM> ram;
  std::unique_ptr<MultiEmu::MemoryRegionROM> rom;

  MultiEmu::MemoryRegionRAM mmu_config = MultiEmu::MemoryRegionRAM(16);

  virtual int run(int cycles) override;
  virtual void draw() override;
  
  static FantacomBoard *get_self(void *ctx);
  static uint8_t read(void *ctx, uint16_t address);
  static void write(void *ctx, uint16_t address, uint8_t value);
  static uint8_t in(void *ctx, uint16_t address);
  static void out(void *ctx, uint16_t address, uint8_t value);
};
