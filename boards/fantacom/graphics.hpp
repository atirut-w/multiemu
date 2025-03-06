#pragma once
#include "multiemu/memory_region.hpp"
#include <cstdint>

struct Graphics {
  MultiEmu::MemoryRegionRAM config;
  MultiEmu::MemoryRegion *ram;

  Graphics() : config(8) {};

  void draw();

  uint32_t get_address(int index) {
    return config.data[index] << 16 | config.data[index + 1] << 8 | config.data[index + 2];
  }
};
