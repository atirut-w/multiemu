#pragma once
#include "multiemu/memory_region.hpp"
#include <cstdint>

struct Graphics {
  MultiEmu::MemoryRegionRAM config;
  MultiEmu::MemoryRegion *ram;
  MultiEmu::MemoryRegionRAM vram;

  Graphics() : config(2), vram(256 * 1024) {
    for (int i = 0; i < 256 * 1024; i++) {
      vram.data[i] = i;
    }
  };

  void draw();
};
