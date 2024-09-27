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
    dirty = true;
  }
}

void Graphics::refresh_charset() {
  if (charset) {
    UnloadRenderTexture(*charset);
  }
  charset = LoadRenderTexture(8, 16 * 256);
  // auto charset_data_addr = get_address(CHARSET_ADDR_IND);
  auto charset_data_addr = data[0] | data[1] << 8 | data[2] << 16;

  BeginTextureMode(*charset);
  ClearBackground(BLACK);

  for (int row = 0; row < 16 * 256; row++) {
    auto charset_row = rambus->read(charset_data_addr + row);
    for (int col = 0; col < 8; col++) {
      auto bit = (charset_row >> (7 - col)) & 1;
      DrawRectangle(col, row, 1, 1, bit ? WHITE : BLACK);
    }
  }

  EndTextureMode();

  auto img = LoadImageFromTexture(charset->texture);
  UnloadTexture(charset->texture);
  ImageFlipVertical(&img);
  charset->texture = LoadTextureFromImage(img);
  UnloadImage(img);
}

void Graphics::draw() {
  auto vram_addr = 0;

  if (dirty) {
    refresh_charset();
    dirty = false;
  }

  // Doing it like an video signal generator is probably the easiest way.
  // 80x25, 8x16 characters, 640x400 pixels
  BeginTextureMode(*Display::framebuffer);
  ClearBackground(BLACK);

  for (int x = 0; x < 80; x++) {
    for (int y = 0; y < 25; y++) {
      auto char_index = rambus->read(vram_addr);
      Rectangle src = {0, (float)(char_index * 16), 8, 16};
      Rectangle dst = {(float)(x * 8), (float)(y * 16), 8, 16};
      DrawTexturePro(charset->texture, src, dst, {0, 0}, 0, WHITE);
      vram_addr++;
    }
  }

  EndTextureMode();
}
