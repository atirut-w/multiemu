// Z80 I/O routines
#pragma once
#include <stdint.h>
#include <io.h>

void outb(uint16_t port, uint8_t data) {
__asm;
  push ix
  ld ix, #0
  add ix, sp

  ld c, (ix+4)
  ld b, (ix+5)
  ld a, (ix+6)
  out (c), a

  pop ix
  ret
__endasm;
}

void outw(uint16_t port, uint16_t data) {
  outb(port, data & 0xff);
  outb(port + 1, data >> 8);
}

void outl(uint16_t port, uint32_t data) {
  outw(port, data & 0xffff);
  outw(port + 2, data >> 16);
}

uint8_t inb(uint16_t port) {
__asm;
  push ix
  ld ix, #0
  add ix, sp

  ld c, (ix+4)
  ld b, (ix+5)
  in a, (c)

  ld l, a
  ld h, #0

  pop ix
  ret
__endasm;
}
