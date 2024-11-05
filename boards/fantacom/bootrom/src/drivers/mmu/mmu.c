#include "mmu/mmu.h"
#include "io.h"
#include <stdio.h>
#include <string.h>

char valid[32];

void mmu_init() { memset(valid, 0, sizeof(valid)); }

int mmu_memcheck() {
  printf("Detecting memory...\n");
  int start = inb(3) + 1;
  int total = 2;
  volatile char *scratch = (char *)0x3000;
  printf("%d", start);

  for (int i = start; i < 256; i++) {
    outb(4, i);
    *scratch = 0x55;
    if (*scratch == 0x55) {
      valid[i / 8] |= 1 << (i % 8);
      total++;
      printf("\r%d KiB", total * 4);
    }
  }

  printf(" Done!\n");
}
