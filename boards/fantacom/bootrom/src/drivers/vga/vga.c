#include "io.h"
#include <stdint.h>
#include <string.h>

#define WIDTH 80
#define HEIGHT 25
#define CHARSET_ADDR_PORT 16
#define SCREEN_ADDR_PORT 20

#define ATTR_DEFAULT 0x07
#define ATTR_CURSOR 0x70
#define TAB_SIZE 8

typedef struct VGAChar {
  uint8_t ch;
  uint8_t attr;
} VGAChar;

extern char font[];
extern VGAChar vram[80 * 25];
int cursor;

void init_vga() {
  cursor = 0;
  outl(CHARSET_ADDR_PORT, (uint32_t)font & 0xffff);
  outl(SCREEN_ADDR_PORT, (uint32_t)vram & 0xffff);
}

int putchar(int ch) {
  vram[cursor].attr = ATTR_DEFAULT;

  switch (ch) {
  default:
    vram[cursor].ch = ch;
    cursor++;
    break;
  case '\n':
    cursor += WIDTH - cursor % WIDTH;
    break;
  case '\r':
    cursor -= cursor % WIDTH;
    break;
  case '\b':
    if (cursor > 0) {
      cursor--;
      vram[cursor].ch = ' ';
      vram[cursor].attr = ATTR_DEFAULT;
    }
    break;
  }

  if (cursor >= WIDTH * HEIGHT) {
    memmove(vram, vram + WIDTH, WIDTH * (HEIGHT - 1) * sizeof(VGAChar));
    memset(vram + WIDTH * (HEIGHT - 1), 0, WIDTH * sizeof(VGAChar));
    cursor = WIDTH * (HEIGHT - 1);
  }

  vram[cursor].attr = ATTR_CURSOR;
  return ch;
}

int puts(const char *str) {
  while (*str) {
    putchar(*str++);
  }
  return 0;
}
