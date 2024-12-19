#include "mmu/mmu.h"
#include "io.h"
#include <stdio.h>
#include <string.h>

char valid[32];
int total_memory;

void mmu_init() {
  memset(valid, 0, sizeof(valid));
  int start = inb(3) + 1;
  int total = 2;
  volatile char *scratch = (char *)0x3000;

  for (int i = start; i < 256; i++) {
    outb(4, i);
    *scratch = 0x55;
    if (*scratch == 0x55) {
      valid[i / 8] |= 1 << (i % 8);
      total++;
    }
  }

  total_memory = total * 4;
}

int mmu_get_total_memory() {
  return total_memory;
}
