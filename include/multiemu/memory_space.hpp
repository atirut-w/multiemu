#pragma once
#include "multiemu/memory_region.hpp"
#include <cstddef>
#include <vector>

namespace MultiEmu {

class MemorySpace {
  struct Mapping {
    MemoryRegion *region;
    std::size_t offset;
    int priority;
  };

  std::size_t size;
  std::vector<Mapping> mappings;

public:
  MemorySpace(std::size_t size) : size(size) {}

  void add_region(MemoryRegion *region, std::size_t offset, int priority) {
    mappings.push_back({region, offset, priority});
  }

  std::uint8_t read(std::size_t offset) const {
    if (offset >= size) {
      return 0;
    }

    const Mapping* candidate = nullptr;
    for (const auto &mapping : mappings) {
      if (offset >= mapping.offset && offset < mapping.offset + mapping.region->get_size()) {
        if (!candidate || mapping.priority > candidate->priority) {
          candidate = &mapping;
        }
      }
    }
    if (candidate) {
      return candidate->region->read(offset - candidate->offset);
    }
    return 0;
  }

  void write(std::size_t offset, std::uint8_t value) {
    if (offset >= size) {
      return;
    }

    const Mapping* candidate = nullptr;
    for (const auto &mapping : mappings) {
      if (offset >= mapping.offset && offset < mapping.offset + mapping.region->get_size()) {
        if (!candidate || mapping.priority > candidate->priority) {
          candidate = &mapping;
        }
      }
    }
    if (candidate) {
      candidate->region->write(offset - candidate->offset, value);
    }
  }
};

}
