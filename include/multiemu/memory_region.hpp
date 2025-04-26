#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>
#include <string>

namespace MultiEmu {

class MemoryRegion {
protected:
  bool backed = false;

public:
  std::vector<MemoryRegion*> subregions; // Made public for address resolution
  size_t offset = 0;  // Offset within parent (set by parent)
  size_t size;        // Size of this region
  int priority = 0;   // Priority for overlapping regions

  MemoryRegion(size_t size) : size(size) {}
  
  virtual ~MemoryRegion() = default;

  // Access functions - access to memory in this region
  virtual uint8_t read(size_t addr) { return 0; }
  virtual void write(size_t addr, uint8_t value) {}

  // No more resolve_region() - that's moved to AddressSpace
  // We'll keep the subregion management for hierarchical memory organization
  void add_subregion(MemoryRegion* region, size_t offset, int priority = 0);
  void remove_subregion(MemoryRegion* region);
  
  // Getters
  bool is_backed() const { return backed; }
};

class MemoryRegionRAM : public MemoryRegion {
private:
  std::vector<uint8_t> data;

public:
  MemoryRegionRAM(size_t size)
    : MemoryRegion(size), data(size, 0) { backed = true; }

  uint8_t read(size_t addr) override { return data[addr]; }
  void write(size_t addr, uint8_t value) override { data[addr] = value; }
  
  // For backward compatibility (temporary)
  std::vector<uint8_t>& get_data() { return data; }
};

class MemoryRegionROM : public MemoryRegion {
private:
  std::vector<uint8_t> data;

public:
  MemoryRegionROM(size_t size) 
    : MemoryRegion(size), data(size, 0) { backed = true; }

  uint8_t read(size_t addr) override { return data[addr]; }
  // ROM doesn't override write - it's read-only and uses the base class implementation

  // Utility method to load ROM data
  void load(const std::vector<uint8_t>& source) {
    size_t copy_size = std::min(source.size(), data.size());
    std::copy(source.begin(), source.begin() + copy_size, data.begin());
  }
  
  // For backward compatibility (temporary)
  std::vector<uint8_t>& get_data() { return data; }
};

class MemoryRegionMMIO : public MemoryRegion {
private:
  std::function<uint8_t(size_t)> read_cb;
  std::function<void(size_t, uint8_t)> write_cb;

public:
  MemoryRegionMMIO(size_t size,
                 std::function<uint8_t(size_t)> read_cb = nullptr,
                 std::function<void(size_t, uint8_t)> write_cb = nullptr)
    : MemoryRegion(size), read_cb(read_cb), write_cb(write_cb) { backed = true; }

  uint8_t read(size_t addr) override {
    return read_cb ? read_cb(addr) : 0;
  }

  void write(size_t addr, uint8_t value) override {
    if (write_cb) write_cb(addr, value);
  }
};

} // namespace MultiEmu
