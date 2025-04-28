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
  MultiEmu::Bus virt;
  MultiEmu::Bus phys;
  MultiEmu::Bus io;

  std::vector<uint8_t> rom;
  std::vector<uint8_t> ram;
  uint8_t mmu_config[0x10];
  
  // Raw pointers for access after ownership transfer
  MultiEmu::Z80* z80_ptr = nullptr;
  Graphics* gfx_ptr = nullptr;
  
  // Constructor to initialize buses with appropriate address space sizes
  FantacomBoard() 
    : virt(0xFFFF),       // 64KB virtual memory
      phys(0xFFFFF),      // 1MB physical memory
      io(0xFF) {          // 256 byte I/O space
    // Create CPU and add it to device hierarchy
    auto cpu = std::make_unique<MultiEmu::Z80>();
    z80_ptr = cpu.get();  // Store raw pointer before ownership transfer
    addChild(std::move(cpu)); // Transfer ownership to device hierarchy
    
    // Create Graphics device and add it to hierarchy
    auto gfx = std::make_unique<Graphics>();
    gfx_ptr = gfx.get();  // Store raw pointer before ownership transfer
    addChild(std::move(gfx)); // Transfer ownership to device hierarchy
  }
  
  // Device interface implementation
  virtual std::string getDeviceName() const override { return "Fantacom"; }
  virtual void reset() override;

  // Board interface
  virtual void setup(const argparse::ArgumentParser &args) override;
  virtual int execute(int cycles) override;
  virtual void draw() override;
  
  // Expose buses for debugging
  virtual std::vector<MultiEmu::BusInfo> get_buses() const override;
  
  // For backward compatibility (gets CPU from device tree now)
  virtual MultiEmu::CPU* getCPU() override { return z80_ptr; }
  
  uint8_t read(uint16_t address);
  void write(uint16_t address, uint8_t value);
  uint8_t in(uint16_t address);
  void out(uint16_t address, uint8_t value);
};
