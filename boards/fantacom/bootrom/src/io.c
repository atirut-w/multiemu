#include <io.h>

__attribute__((naked)) uint8_t in(uint16_t address) {
  __asm__(
    "push ix\n"
    "ld ix, 0\n"
    "add ix, sp\n"

    "ld c, (ix + 4)\n"
    "ld b, (ix + 5)\n"
    "in a, (c)\n"

    "ld l, a\n"
    
    "ld sp, ix\n"
    "pop ix\n"
    "ret\n"
  );
}

__attribute__((naked)) void out(uint16_t address, uint8_t value) {
  __asm__(
    "push ix\n"
    "ld ix, 0\n"
    "add ix, sp\n"

    "ld c, (ix + 4)\n"
    "ld b, (ix + 5)\n"
    "ld a, (ix + 6)\n"
    "out (c), a\n"

    "ld sp, ix\n"
    "pop ix\n"
    "ret\n"
  );
}

void out16(uint16_t address, uint16_t value) {
  out(address, value & 0xff);
  out(address + 1, value >> 8);
}

void out32(uint16_t address, uint32_t value) {
  out16(address, value & 0xffff);
  out16(address + 2, value >> 16);
}
