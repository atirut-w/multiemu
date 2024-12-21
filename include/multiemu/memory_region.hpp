#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace MultiEmu {

class MemoryRegion {
  std::size_t size;
  int priority;
  std::vector<MemoryRegion *> subregions;

public:
  std::size_t offset;

  MemoryRegion(std::size_t size) : size(size) {};

  virtual uint8_t read(std::size_t addr);
  virtual void write(std::size_t addr, uint8_t value);

  void add_subregion(MemoryRegion *region, std::size_t offset, int priority = 0);
  void remove_subregion(MemoryRegion *region);
  MemoryRegion *resolve_address(std::size_t addr);
};

class MemoryRegionContainer : public MemoryRegion {
public:
  MemoryRegionContainer(std::size_t size) : MemoryRegion(size) {};

  uint8_t read(std::size_t addr) override;
  void write(std::size_t addr, uint8_t value) override;
};

class MemoryRegionRAM : public MemoryRegion {
  std::vector<uint8_t> data;

public:
  MemoryRegionRAM(std::size_t size) : MemoryRegion(size), data(size) {};

  uint8_t read(std::size_t addr) override;
  void write(std::size_t addr, uint8_t value) override;
};

class MemoryRegionROM : public MemoryRegion {
  std::vector<uint8_t> data;

public:
  MemoryRegionROM(const std::vector<uint8_t> &data) : MemoryRegion(data.size()), data(data) {};

  uint8_t read(std::size_t addr) override;
  void write(std::size_t addr, uint8_t value) override;
};

class MMIOOps {
public:
  std::function<uint8_t(std::size_t)> read = [](std::size_t) { return 0; };
  std::function<void(std::size_t, uint8_t)> write = [](std::size_t, uint8_t) {};
};

class MemoryRegionMMIO : public MemoryRegion {
  MMIOOps ops;

public:
  MemoryRegionMMIO(std::size_t size, MMIOOps ops) : MemoryRegion(size), ops(ops) {};

  uint8_t read(std::size_t addr) override;
  void write(std::size_t addr, uint8_t value) override;
};

}
