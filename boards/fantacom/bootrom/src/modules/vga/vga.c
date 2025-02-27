#include "io.h"
#include <stdint.h>

#define CHARSET_ADDR_PORT 16
#define SCREEN_ADDR_PORT 20

typedef struct {
  uint8_t ch;
  uint8_t attr;
} VGAChar;

extern char font[];
VGAChar vram[80 * 25];

__attribute__((constructor)) void vga_init(void) {
  out32(CHARSET_ADDR_PORT, (uint32_t)font & 0xffff);
  out32(SCREEN_ADDR_PORT, (uint32_t)vram & 0xffff);
}
