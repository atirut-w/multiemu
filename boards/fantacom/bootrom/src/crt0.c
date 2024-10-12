#include "io.h"

extern char font;

void crt0_start() {
  // outb(0x16, 0);
  outb(0x17, 0);
  outb(0x18, 0);
  outb(0x19, 0);

  while (1) {
    for (int i = 0; i < 16; i++) {
      outb(0x16, i);
    }
  }

  while (1) {} // Stop here
}
