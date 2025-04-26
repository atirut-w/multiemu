#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <utility>
#include "memory_region.hpp"

namespace MultiEmu {

enum class AddressSpaceType {
  MEMORY, // Default unified memory (von Neumann)
  IO,     // I/O space
  CUSTOM  // For any other address spaces
};

class AddressSpace {
private:
  std::vector<MemoryRegion*> regions;

  // Helper for recursive resolution into subregions
  std::pair<MemoryRegion*, size_t> resolve_region_recursive(size_t addr);

public:
  std::string name;        // Made public for UI access
  AddressSpaceType type;
  size_t size;             // Made public for UI access
  
  // Program counter getter for memory viewer/debugger
  std::function<size_t()> getProgramCounter = nullptr;

  AddressSpace(const std::string &name, AddressSpaceType type, size_t size)
      : name(name), type(type), size(size) {}
  
  ~AddressSpace() = default;

  // Basic memory operations
  uint8_t read(size_t addr);
  void write(size_t addr, uint8_t value);

  // Region management
  void add_region(MemoryRegion* region, size_t offset, int priority = 0);
  void remove_region(MemoryRegion* region);
  MemoryRegion* resolve_region(size_t addr);
};

} // namespace MultiEmu