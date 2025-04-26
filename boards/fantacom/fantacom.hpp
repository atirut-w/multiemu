#pragma once
#include "argparse/argparse.hpp"
#include "cpu/z80.hpp"
#include "fantacom/graphics.hpp"
#include "multiemu/address_space.hpp"
#include "multiemu/board.hpp"
#include "multiemu/memory_region.hpp"
#include <cstdint>
#include <memory>

class FantacomBoard : public MultiEmu::Board {
private:
  // Memory regions
  std::unique_ptr<MultiEmu::MemoryRegionROM> rom_region;
  std::unique_ptr<MultiEmu::MemoryRegionRAM> ram_region;
  std::unique_ptr<MultiEmu::MemoryRegion> io_region;
  std::unique_ptr<MultiEmu::MemoryRegionMMIO> virt_mmio;

public:
  Graphics gfx;

  // Address spaces
  MultiEmu::AddressSpace virt;
  MultiEmu::AddressSpace phys; 
  MultiEmu::AddressSpace io;

  // Global MMU config
  MultiEmu::MemoryRegionRAM mmu_config{16};
  
  // Constructor to initialize address spaces
  FantacomBoard() : 
    virt("Memory (Virtual)", MultiEmu::AddressSpaceType::MEMORY, 64 * 1024),
    phys("Memory (Physical)", MultiEmu::AddressSpaceType::MEMORY, 1024 * 1024),
    io("I/O", MultiEmu::AddressSpaceType::IO, 256)
  {}

  virtual void setup(const argparse::ArgumentParser &args) override;
  virtual int run(int cycles) override;
  virtual void draw() override;
  
  uint8_t read(uint16_t address);
  void write(uint16_t address, uint8_t value);
  uint8_t in(uint16_t address);
  void out(uint16_t address, uint8_t value);
};
