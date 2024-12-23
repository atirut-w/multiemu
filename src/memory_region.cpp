#include "multiemu/memory_region.hpp"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace MultiEmu;

uint8_t MemoryRegion::read(size_t addr) {
  auto region = resolve_address(addr);
  if (!region) {
    cout << "WARN: read from invalid address " << addr << "\n";
    return 0;
  }
  return region->read(addr - region->offset);
}

void MemoryRegion::write(size_t addr, uint8_t value) {
  auto region = resolve_address(addr);
  if (region) {
    region->write(addr - region->offset, value);
  } else {
    cout << "WARN: write to invalid address " << addr << "\n";
  }
}

void MemoryRegion::add_subregion(MemoryRegion *region, size_t offset, int priority) {
  if (offset + region->size > size) {
    throw invalid_argument("subregion does not fit in parent region");
  }
  region->offset = offset;
  region->priority = priority;
  subregions.push_back(region);
  sort(subregions.begin(), subregions.end(),
       [](MemoryRegion *a, MemoryRegion *b) { return a->priority > b->priority; });
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
      auto resolved = region->resolve_address(rel);
      if (resolved) {
        return resolved;
      }
    }
  }
  return backed ? this : nullptr;
}

uint8_t MemoryRegionRAM::read(size_t addr) {
  auto region = resolve_address(addr);
  if (region != this) {
    return region->read(addr - region->offset);
  }
  return data[addr];
}

void MemoryRegionRAM::write(size_t addr, uint8_t value) {
  auto region = resolve_address(addr);
  if (region != this) {
    region->write(addr - region->offset, value);
  } else {
    data[addr] = value;
  }
}

uint8_t MemoryRegionROM::read(size_t addr) {
  auto region = resolve_address(addr);
  if (region != this) {
    return region->read(addr - region->offset);
  }
  return data[addr];
}

void MemoryRegionROM::write(size_t addr, uint8_t value) {
  // NOP
}

uint8_t MemoryRegionMMIO::read(size_t addr) {
  auto region = resolve_address(addr);
  if (region != this) {
    return region->read(addr - region->offset);
  }
  return ops.read(addr);
}

void MemoryRegionMMIO::write(size_t addr, uint8_t value) {
  auto region = resolve_address(addr);
  if (region != this) {
    region->write(addr - region->offset, value);
  } else {
    ops.write(addr, value);
  }
}
