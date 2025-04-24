#pragma once
#include "argparse/argparse.hpp"
#include "cpu/z80.hpp"
#include "fantacom/graphics.hpp"
#include "multiemu/board.hpp"
#include "multiemu/memory_region.hpp"
#include <cstdint>
#include <memory>

class FantacomBoard : public MultiEmu::Board {
public:
  Graphics gfx;

  std::unique_ptr<MultiEmu::MemoryRegion> phys;
  std::unique_ptr<MultiEmu::MemoryRegion> io;
  std::unique_ptr<MultiEmu::MemoryRegionRAM> ram;
  std::unique_ptr<MultiEmu::MemoryRegionROM> rom;

  MultiEmu::MemoryRegionRAM mmu_config = MultiEmu::MemoryRegionRAM(16);

  virtual void setup(const argparse::ArgumentParser &args) override;
  virtual int run(int cycles) override;
  virtual void draw() override;
  
  uint8_t read(uint16_t address);
  void write(uint16_t address, uint8_t value);
  uint8_t in(uint16_t address);
  void out(uint16_t address, uint8_t value);
};
