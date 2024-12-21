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
  MemoryRegion(std::size_t size) : size(size) {};
  virtual uint8_t read(std::size_t addr);
  virtual void write(std::size_t addr, uint8_t value);

  void add_subregion(MemoryRegion *region, std::size_t offset, int priority = 0);
  void remove_subregion(MemoryRegion *region);
  virtual MemoryRegion *resolve_address(std::size_t addr);
};

}
