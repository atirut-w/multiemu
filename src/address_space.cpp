#include "multiemu/address_space.hpp"
#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace MultiEmu {

uint8_t AddressSpace::read(size_t addr) {
  if (addr >= size) {
    std::cout << "WARN: read from out of bounds address " << addr << " in address space " << name << "\n";
    return 0;
  }

  // Find the region and relative address within it
  auto [region, region_addr] = resolve_region_recursive(addr);
  
  if (!region) {
    std::cout << "WARN: read from unmapped address " << addr << " in address space " << name << "\n";
    return 0;
  }

  // Read from the final resolved region
  return region->read(region_addr);
}

void AddressSpace::write(size_t addr, uint8_t value) {
  if (addr >= size) {
    std::cout << "WARN: write to out of bounds address " << addr << " in address space " << name << "\n";
    return;
  }

  // Find the region and relative address within it
  auto [region, region_addr] = resolve_region_recursive(addr);
  
  if (!region) {
    std::cout << "WARN: write to unmapped address " << addr << " in address space " << name << "\n";
    return;
  }

  // Write to the final resolved region
  region->write(region_addr, value);
}

void AddressSpace::add_region(MemoryRegion* region, size_t offset, int priority) {
  if (offset + region->size > size) {
    throw std::invalid_argument("Region does not fit in address space");
  }

  region->offset = offset;
  region->priority = priority;
  regions.push_back(region);

  // Sort regions by priority (higher priority first)
  std::sort(regions.begin(), regions.end(),
       [](MemoryRegion* a, MemoryRegion* b) { return a->priority > b->priority; });
}

void AddressSpace::remove_region(MemoryRegion* region) {
  auto it = std::find(regions.begin(), regions.end(), region);
  if (it == regions.end()) {
    throw std::invalid_argument("Region not found in address space");
  }
  regions.erase(it);
}

MemoryRegion* AddressSpace::resolve_region(size_t addr) {
  if (addr >= size) {
    std::cout << "WARN: address " << addr << " out of bounds for address space " << name << "\n";
    return nullptr;
  }

  // Check all regions in priority order
  for (auto& region : regions) {
    if (addr >= region->offset && addr < region->offset + region->size) {
      return region;
    }
  }

  return nullptr;
}

std::pair<MemoryRegion*, size_t> AddressSpace::resolve_region_recursive(size_t addr) {
  // Find the top-level region
  MemoryRegion* region = resolve_region(addr);
  if (!region) {
    return {nullptr, 0};
  }
  
  // Calculate address relative to this region
  size_t rel_addr = addr - region->offset;
  
  // Simplified flattened recursive search
  MemoryRegion* current = region;
  size_t current_addr = rel_addr;
  
  // Keep looking for the deepest matching subregion
  bool found_subregion;
  do {
    found_subregion = false;
    
    // Try to find a matching subregion
    for (auto& sub : current->subregions) {
      if (current_addr >= sub->offset && current_addr < sub->offset + sub->size) {
        // Found a subregion - update current and continue search
        current_addr = current_addr - sub->offset;
        current = sub;
        found_subregion = true;
        break;
      }
    }
  } while (found_subregion);
  
  // Return the final region if it's backed
  return current->is_backed() ? 
    std::make_pair(current, current_addr) : 
    std::make_pair(nullptr, 0);
}

} // namespace MultiEmu