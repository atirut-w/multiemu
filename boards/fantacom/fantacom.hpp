#pragma once
#include "argparse/argparse.hpp"
#include "cpu/z80.hpp"
#include "fantacom/graphics.hpp"
#include "multiemu/board.hpp"
#include <cstdint>
#include <memory>
#include <multiemu//bus.hpp>
#include <vector>

class FantacomBoard : public MultiEmu::Board {
public:
  Graphics gfx;

  MultiEmu::Bus<uint16_t, uint8_t> virt;
  MultiEmu::Bus<uint32_t, uint8_t> phys;
  MultiEmu::Bus<uint8_t, uint8_t> io;

  std::vector<uint8_t> rom;
  std::vector<uint8_t> ram;
  uint8_t mmu_config[0x10];
  
  // Constructor to initialize address spaces
  FantacomBoard() {}

  virtual void setup(const argparse::ArgumentParser &args) override;
  virtual int run(int cycles) override;
  virtual void draw() override;
  
  // Expose buses for debugging
  virtual std::vector<MultiEmu::BusInfo> get_buses() const override;
  
  uint8_t read(uint16_t address);
  void write(uint16_t address, uint8_t value);
  uint8_t in(uint16_t address);
  void out(uint16_t address, uint8_t value);
};
