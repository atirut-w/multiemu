#pragma once
#include "multiemu/memory_region.hpp"
#include <cstdint>

struct Graphics {
  MultiEmu::MemoryRegionRAM config;
  MultiEmu::MemoryRegionRAM vram;

  Graphics() : 
    config(2),
    vram(256 * 1024) {
    // Initialize VRAM with a pattern
    for (int i = 0; i < 256 * 1024; i++) {
      vram.write(i, i & 0xFF);
    }
  };

  void draw();
  void drawTextMode80x25();
};
