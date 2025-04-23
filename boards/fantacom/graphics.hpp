#pragma once
#include "multiemu/memory_region.hpp"
#include <cstdint>

struct Graphics {
  MultiEmu::MemoryRegionRAM config;
  MultiEmu::MemoryRegion *ram;

  Graphics() : config(2) {};

  void draw();

  uint32_t get_address(int index) {
    // Use page-based addressing like the MMU
    // Each entry is a page number, addressing starts at beginning of page
    uint8_t page = config.data[index];
    return page << 12; // Convert page to physical address (4KB pages)
  }
};
