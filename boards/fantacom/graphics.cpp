#include "fantacom/graphics.hpp"
#include "multiemu/display.hpp"
#include "raylib.h"
#include <array>
#include <cstdint>

using namespace std;
using namespace MultiEmu;

array<Color, 16> palette = {
    Color{0, 0, 0, 255},       // Black
    {0, 0, 170, 255},     // Blue
    {0, 170, 0, 255},     // Green
    {0, 170, 170, 255},   // Cyan
    {170, 0, 0, 255},     // Red
    {170, 0, 170, 255},   // Magenta
    {170, 85, 0, 255},    // Brown
    {170, 170, 170, 255}, // Light Gray
    {85, 85, 85, 255},    // Dark Gray
    {85, 85, 255, 255},   // Light Blue
    {85, 255, 85, 255},   // Light Green
    {85, 255, 255, 255},  // Light Cyan
    {255, 85, 85, 255},   // Light Red
    {255, 85, 255, 255},  // Light Magenta
    {255, 255, 85, 255},  // Yellow
    {255, 255, 255, 255}  // White
};

uint8_t Graphics::read(std::size_t addr) {
  return addr < data.size() ? data[addr] : 0;
}

void Graphics::write(std::size_t addr, uint8_t data) {
  if (addr < this->data.size()) {
    this->data[addr] = data;
  }
}

void Graphics::draw() {
  auto charset_addr = *(uint32_t *)&data[0];
  auto charset_img = GenImageColor(8, 16 * 256, BLACK);
  for (int row = 0; row < 16 * 256; row++) {
    auto char_row = rambus->read(charset_addr++);
    for (int col = 0; col < 8; col++) {
      auto bit = (char_row >> (7 - col)) & 1;
      ImageDrawPixel(&charset_img, col, row, bit ? WHITE : Color{0, 0, 0, 0});
    }
  }

  auto charset_tex = LoadTextureFromImage(charset_img);
  UnloadImage(charset_img);

  BeginTextureMode(*Display::framebuffer);
  ClearBackground(BLACK);

  // 80x25, 8x16
  auto vram_addr = *(uint32_t *)&data[4];
  for (int cy = 0; cy < 25; cy++) {
    for (int cx = 0; cx < 80; cx++) {
      auto char_index = rambus->read(vram_addr++);
      auto char_attr = rambus->read(vram_addr++);
      auto fg = palette[char_attr & 0x0f];
      auto bg = palette[char_attr >> 4];

      Rectangle src = {0, (float)char_index * 16, 8, 16};
      Rectangle dst = {(float)cx * 8, (float)cy * 16, 8, 16};
      DrawRectangle(dst.x, dst.y, dst.width, dst.height, bg);
      DrawTexturePro(charset_tex, src, dst, {0, 0}, 0, fg);
    }
  }

  EndTextureMode();
  UnloadTexture(charset_tex);
}
