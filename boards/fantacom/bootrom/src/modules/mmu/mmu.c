#include "mmu/mmu.h"
#include "io.h"

#define PAGE_TABLE 0x0000

uint32_t mmu_get_physical(void *addr) {
  uint8_t vpage = (uint16_t)addr >> 12;
  uint8_t ppage = in(PAGE_TABLE + vpage);
  return (uint32_t)ppage << 12 | ((uint16_t)addr & 0x0fff);
}
