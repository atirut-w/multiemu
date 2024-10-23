#include "io.h"
#include <stdint.h>

extern char font[];
extern uint8_t vram[];

void init_vga() {
  outl(16, (uint32_t)&font & 0xffff);
  outl(20, (uint32_t)&vram & 0xffff);
}
