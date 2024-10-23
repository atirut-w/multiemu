#include "io.h"
#include <stdint.h>

typedef struct VGAChar {
  uint8_t ch;
  uint8_t attr;
} VGAChar;

extern char font[];
extern VGAChar vram[80 * 25];
VGAChar *cursor;

void init_vga() {
  cursor = vram;
  outl(16, (uint32_t)font & 0xffff);
  outl(20, (uint32_t)vram & 0xffff);
}

int putchar(int ch) {
  cursor->ch = ch;
  cursor->attr = 0x07;
  cursor++;
}

int puts(const char *str) {
  while (*str) {
    putchar(*str++);
  }
}
