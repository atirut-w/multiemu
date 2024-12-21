#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

namespace MultiEmu {

class MemoryRegion {
  std::size_t offset;
  std::size_t size;
  int priority;
  std::vector<MemoryRegion *> subregions;

public:
  virtual uint8_t read(std::size_t addr) const = 0;
  virtual void write(std::size_t addr, uint8_t value) = 0;

  void add_subregion(MemoryRegion *region, std::size_t offset, int priority = 0);
  void remove_subregion(MemoryRegion *region);
  virtual MemoryRegion *resolve_address(std::size_t addr);
};

}
