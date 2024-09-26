#include "fantacom/mmu.hpp"
#include <cstdint>

uint8_t MMU::read(std::size_t addr) {
  if (addr < 16) {
    return pagemap[addr];
  }
  return 0;
}

void MMU::write(std::size_t addr, uint8_t data) {
  if (addr < 16) {
    pagemap[addr] = data;
  }
}

std::uint32_t MMU::translate(std::uint32_t addr) {
  int page = addr >> 12;
  return (pagemap[page] << 12) | (addr & 0xfff);
}
