#include "io.h"
#include "mmu/mmu.h"
#include <stdint.h>
#include <stdio.h>

int main() {
  printf("Fantacom boot ROM, (C) Atirut Wattanamongkol and contributors\n\n");
  printf("Total memory: %d KiB\n", mmu_get_total_memory());
  return 0;
}
