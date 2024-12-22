#pragma once
#include "multiemu/memory_region.hpp"
#include "multiemu/units.hpp"
#include <array>
#include <cstdint>

struct MemoryRegionMMU : MultiEmu::MemoryRegion {
  MultiEmu::MemoryRegionRAM config;
  MultiEmu::MemoryRegion *physical;

  MemoryRegionMMU() : MultiEmu::MemoryRegion(64 * KIB), config(16) {};

  uint8_t read(size_t addr) override;
  void write(size_t addr, uint8_t value) override;
};
