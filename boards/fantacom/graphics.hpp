#pragma once
#include "multiemu/memory_region.hpp"
#include <cstdint>

struct Graphics {
  MultiEmu::MemoryRegionMMIO config;
  MultiEmu::MemoryRegionRAM vram;

  int mode = 1; // Graphics mode

  Graphics()
      : config(
            1,
            [this](size_t addr) {
              if (addr == 0) {
                return mode;
              }
              return 0;
            },
            [&](size_t addr, uint8_t value) {
              if (addr == 0) {
                mode = value;
                modeset();
              }
            }),
        vram(256 * 1024) {
    // Initialize VRAM with a pattern
    for (int i = 0; i < 256 * 1024; i++) {
      vram.write(i, i & 0xFF);
    }

    modeset();
  };

  void modeset();
  void draw();

  // Draw in VGA-like 80x25 text mode
  void drawTextMode80x25();
  // Draw in EGA-like 320x200x16 bitmap mode
  void drawBitmapMode320x200x16();
};
