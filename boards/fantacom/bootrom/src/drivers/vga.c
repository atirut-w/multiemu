#include <stdint.h>
#include "io.h"

typedef struct VGAChar {
  uint8_t ch;
  uint8_t attr;
} VGAChar;

extern char font[];
extern VGAChar vram[80 * 25];
int cursor;

void init_vga() {
  cursor = 0;
  outl(16, (uint32_t)font & 0xffff);
  outl(20, (uint32_t)vram & 0xffff);
}

int putchar(int ch) {
  vram[cursor].attr = 0x07;

  switch (ch) {
  default:
    vram[cursor].ch = ch;
    cursor++;
    break;
  case '\n': {
    cursor += 80 - cursor % 80;
    break;
  }
  }

  vram[cursor].attr = 0x70;
}

int puts(const char *str) {
  while (*str) {
    putchar(*str++);
  }
}
