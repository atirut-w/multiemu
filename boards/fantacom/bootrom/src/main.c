#include "io.h"
#include "mmu/mmu.h"
#include <stdint.h>
#include <stdio.h>

int main() {
  printf("Fantacom boot ROM, (C) Atirut Wattanamongkol and contributors\n\n");
  mmu_probe();
  return 0;
}
