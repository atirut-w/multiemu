#include "output.h"

__attribute__((aligned(256))) static void (*handlers[128])();

__attribute__((interrupt)) void bad_int() {
  // Handle invalid interrupt
  // This is a placeholder for actual interrupt handling logic
  puts("Invalid interrupt");
}

__attribute__((constructor)) static void init_mode2() {
  // Initialize the interrupt handlers
  for (int i = 0; i < 128; ++i) {
    handlers[i] = bad_int;
  }

  asm volatile (
    "ld hl, _handlers\n"
    "ld a, h\n"
    "ld i, a\n"
    "im 2\n"
    :
    :
    : "hl", "a"
  );
}
