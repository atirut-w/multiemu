#include "fantacom/graphics.hpp"
#include "multiemu/display.hpp"
#include "raylib.h"
#include <cstdint>

using namespace std;
using namespace MultiEmu;

uint8_t Graphics::read(std::size_t addr) {
  return addr < data.size() ? data[addr] : 0;
}

void Graphics::write(std::size_t addr, uint8_t data) {
  if (addr < this->data.size()) {
    this->data[addr] = data;
  }
}

void Graphics::draw() {
  auto charset_addr = data[0] | data[1] << 8 | data[2] << 16;
  auto vram_addr = data[3] | data[4] << 8 | data[5] << 16;

  BeginTextureMode(*Display::framebuffer);
  ClearBackground(BLACK);

  // 80x25, 8x16
  for (int cx = 0; cx < 80; cx++) {
    for (int cy = 0; cy < 25; cy++) {
      auto char_index = rambus->read(vram_addr++);
      auto char_addr = charset_addr + char_index * 16;
      int bx = cx * 8;
      int by = cy * 16;

      for (int row = 0; row < 16; row++) {
        auto char_row = rambus->read(char_addr + row);
        for (int col = 0; col < 8; col++) {
          auto bit = (char_row >> (7 - col)) & 1;
          DrawPixel(bx + col, by + row, bit ? WHITE : BLACK);
        }
      }
    }
  }

  EndTextureMode();
}
