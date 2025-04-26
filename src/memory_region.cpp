#include "multiemu/memory_region.hpp"
#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace MultiEmu {

void MemoryRegion::add_subregion(MemoryRegion* region, size_t offset, int priority) {
  if (offset + region->size > size) {
    throw std::invalid_argument("Subregion does not fit in parent region");
  }
  
  region->offset = offset;
  region->priority = priority;
  subregions.push_back(region);
  
  // Sort subregions by priority (higher priority first)
  std::sort(subregions.begin(), subregions.end(),
       [](MemoryRegion* a, MemoryRegion* b) { return a->priority > b->priority; });
}

void MemoryRegion::remove_subregion(MemoryRegion* region) {
  auto it = std::find(subregions.begin(), subregions.end(), region);
  if (it == subregions.end()) {
    throw std::invalid_argument("Subregion not found");
  }
  subregions.erase(it);
}

} // namespace MultiEmu