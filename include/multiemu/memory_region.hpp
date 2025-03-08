#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace MultiEmu {

class MemoryRegion {
protected:
  int priority;
  bool backed = false;
  std::vector<MemoryRegion *> subregions;

public:
  std::size_t offset;
  std::size_t size;

  MemoryRegion(std::size_t size) : size(size) {};

  virtual uint8_t read(std::size_t addr);
  virtual void write(std::size_t addr, uint8_t value);
  
  // Helper methods to reduce duplication in derived classes
  virtual uint8_t read_internal(std::size_t addr) { return 0; }
  virtual void write_internal(std::size_t addr, uint8_t value) {}

  void add_subregion(MemoryRegion *region, std::size_t offset, int priority = 0);
  void remove_subregion(MemoryRegion *region);
  virtual MemoryRegion *resolve_address(std::size_t addr);
};

class MemoryRegionRAM : public MemoryRegion {
public:
  std::vector<uint8_t> data;

  MemoryRegionRAM(std::size_t size) : MemoryRegion(size), data(size) { backed = true; };

  uint8_t read_internal(std::size_t addr) override { return data[addr]; }
  void write_internal(std::size_t addr, uint8_t value) override { data[addr] = value; }
};

class MemoryRegionROM : public MemoryRegion {
public:
  std::vector<uint8_t> data;

  MemoryRegionROM(std::size_t size) : MemoryRegion(size), data(size) { backed = true; };

  uint8_t read_internal(std::size_t addr) override { return data[addr]; }
  // No write_internal override for ROM - it's read-only
};

class MemoryRegionMMIO : public MemoryRegion {
  std::function<uint8_t(std::size_t)> read_cb = nullptr;
  std::function<void(std::size_t, uint8_t)> write_cb = nullptr;

public:
  MemoryRegionMMIO(std::size_t size, 
                  std::function<uint8_t(std::size_t)> read_cb = nullptr,
                  std::function<void(std::size_t, uint8_t)> write_cb = nullptr) 
    : MemoryRegion(size), read_cb(read_cb), write_cb(write_cb) { backed = true; };

  uint8_t read_internal(std::size_t addr) override { 
    return read_cb ? read_cb(addr) : 0; 
  }
  
  void write_internal(std::size_t addr, uint8_t value) override { 
    if (write_cb) write_cb(addr, value);
  }
};

}
