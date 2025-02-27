#include "io.h"
#include <stdint.h>
#include "mmu/mmu.h"

#define WIDTH 80
#define HEIGHT 25
#define CHARSET_ADDR_PORT 16
#define SCREEN_ADDR_PORT 20

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
