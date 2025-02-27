#include "io.h"
#include "mmu/mmu.h"
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define WIDTH 80
#define HEIGHT 25
#define CHARSET_ADDR_PORT 16
#define SCREEN_ADDR_PORT 20

#define ATTR_DEFAULT 0x07
#define ATTR_CURSOR 0x70
#define TAB_SIZE 8

typedef struct {
  uint8_t ch;
  uint8_t attr;
} VGAChar;

extern char font[];
VGAChar vram[WIDTH * HEIGHT];
int cursor = 0;

__attribute__((constructor)) void vga_init(void) {
  out32(CHARSET_ADDR_PORT, mmu_get_physical(font));
  out32(SCREEN_ADDR_PORT, mmu_get_physical(vram));
}

ssize_t write(int fd, const void *buf, size_t count) {
  const char *str = buf;
  size_t i;

  if (fd != 1) {
    return -1;
  }

  vram[cursor].attr = ATTR_DEFAULT;

  for (i = 0; i < count; i++) {
    /* This implement C0 control characters */
    switch (str[i]) {
    case '\a':
      /* Bell */
      break;
    case '\b':
      /* Backspace */
      if (cursor > 0) {
      cursor--;
      }
      break;
    case '\t':
      /* Horizontal tab */
      cursor += TAB_SIZE - (cursor % TAB_SIZE);
      break;
    case '\n':
      /* Line feed */
      cursor += WIDTH;
      break;
    case '\f':
      /* Form feed */
      cursor = cursor % WIDTH;
      break;
    case '\r':
      /* Carriage return */
      cursor -= cursor % WIDTH;
      break;
    default:
      vram[cursor].ch = str[i];
      vram[cursor].attr = ATTR_DEFAULT;
      cursor++;
    }

    if (cursor >= WIDTH * HEIGHT) {
      memmove(vram, vram + WIDTH, WIDTH * (HEIGHT - 1) * sizeof(VGAChar));
      memset(vram + WIDTH * (HEIGHT - 1), 0, WIDTH * sizeof(VGAChar));
      cursor -= WIDTH;
    }
  }

  vram[cursor].attr = ATTR_CURSOR;
  return count;
}
