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
  auto charset_img = GenImageColor(8, 16 * 256, BLACK);
  for (int row = 0; row < 16 * 256; row++) {
    auto char_row = rambus->read(charset_addr++);
    for (int col = 0; col < 8; col++) {
      auto bit = (char_row >> (7 - col)) & 1;
      ImageDrawPixel(&charset_img, col, row, bit ? WHITE : BLACK);
    }
  }

  auto charset_tex = LoadTextureFromImage(charset_img);
  UnloadImage(charset_img);

  BeginTextureMode(*Display::framebuffer);
  ClearBackground(BLACK);

  // 80x25, 8x16
  auto vram_addr = data[3] | data[4] << 8 | data[5] << 16;
  for (int cy = 0; cy < 25; cy++) {
    for (int cx = 0; cx < 80; cx++) {
      auto char_index = rambus->read(vram_addr++);
      Rectangle src = {0, (float)char_index * 16, 8, 16};
      Rectangle dst = {(float)cx * 8, (float)cy * 16, 8, 16};
      DrawTexturePro(charset_tex, src, dst, {0, 0}, 0, WHITE);
    }
  }

  EndTextureMode();
  UnloadTexture(charset_tex);
}
