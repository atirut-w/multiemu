#include "multiemu/memory_region.hpp"
#include <algorithm>
#include <cstddef>
#include <stdexcept>

using namespace std;
using namespace MultiEmu;

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
    int rel = addr - region->offset;
    if (rel >= 0 && rel < region->size) {
      auto subregion = region->resolve_address(rel);
      if (subregion) {
        return subregion;
      }
    }
  }
  return nullptr;
}
