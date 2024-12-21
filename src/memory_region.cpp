#include "multiemu/memory_region.hpp"
#include <algorithm>
#include <cstddef>
#include <stdexcept>

using namespace std;
using namespace MultiEmu;

uint8_t MemoryRegion::read(size_t addr) {
  auto region = resolve_address(addr);
  if (region == this) {
    throw out_of_range("address not mapped");
  }
  return region->read(addr - region->offset);
}

void MemoryRegion::write(size_t addr, uint8_t value) {
  auto region = resolve_address(addr);
  if (region == this) {
    throw out_of_range("address not mapped");
  }
  region->write(addr - region->offset, value);
}

void MemoryRegion::add_subregion(MemoryRegion *region, size_t offset, int priority) {
  if (offset + region->size > size) {
    throw invalid_argument("subregion does not fit in parent region");
  }
  region->offset = offset;
  region->priority = priority;
  subregions.push_back(region);
  // Sort subregions by descending priority
  sort(subregions.begin(), subregions.end(), [](MemoryRegion *a, MemoryRegion *b) {
    return a->priority > b->priority;
  });
}

void MemoryRegion::remove_subregion(MemoryRegion *region) {
  auto it = find(subregions.begin(), subregions.end(), region);
  if (it == subregions.end()) {
    throw invalid_argument("subregion not found");
  }
  subregions.erase(it);
}

MemoryRegion *MemoryRegion::resolve_address(size_t addr) {
  if (addr >= size) {
    throw out_of_range("address out of range");
  }
  for (auto region : subregions) {
    size_t rel = addr - region->offset;
    if (rel < region->size) {
      return region->resolve_address(rel);
    }
  }
  return this;
}
