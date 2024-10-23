#include "io.h"
#include <stdint.h>

extern char font;
const uint8_t vga_data[] = {
  'H',
  0x07,
  'e',
  0x07,
  'l',
  0x07,
  'l',
  0x07,
  'o',
  0x07,
  ' ',
  0x07,
  'W',
  0x07,
  'o',
  0x07,
  'r',
  0x07,
  'l',
  0x07,
  'd',
  0x07,
  '!',
  0x07,
};

int main() {
  outl(16, (uint32_t)&font & 0xffff);
  outl(20, (uint32_t)&vga_data & 0xffff);
  return 0;
}
