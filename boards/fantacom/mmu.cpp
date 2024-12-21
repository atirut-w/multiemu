#include "fantacom/mmu.hpp"
#include <cstdint>

uint8_t MemoryRegionMMU::read(std::size_t addr) {
  int v_page = addr >> 12;
  int p_page = config.read(v_page);
  int p_addr = (p_page << 12) | (addr & 0xfff);
  return physical->read(p_addr);
}

void MemoryRegionMMU::write(std::size_t addr, uint8_t data) {
  int v_page = addr >> 12;
  int p_page = config.read(v_page);
  int p_addr = (p_page << 12) | (addr & 0xfff);
  physical->write(p_addr, data);
}
