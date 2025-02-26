#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace MultiEmu {

class MemoryRegion {
protected:
  std::optional<std::string> name;

public:
  virtual std::size_t get_size() const = 0;
  virtual std::uint8_t read(std::size_t offset) const { return 0; }
  virtual void write(std::size_t offset, std::uint8_t value) {}
};

class MemoryRegionRAM : public MemoryRegion {
public:
  std::vector<std::uint8_t> data;

  MemoryRegionRAM(std::size_t size) : data(size) {}

  std::size_t get_size() const override { return data.size(); }
  std::uint8_t read(std::size_t offset) const override { return data[offset]; }
  void write(std::size_t offset, std::uint8_t value) override { data[offset] = value; }
};

class MemoryRegionROM : public MemoryRegion {
public:
  std::vector<std::uint8_t> data;

  MemoryRegionROM(std::size_t size) : data(size) {}

  std::size_t get_size() const override { return data.size(); }
  std::uint8_t read(std::size_t offset) const override { return data[offset]; }
};

class MemoryRegionMMIO : public MemoryRegion {
public:
  std::function<std::uint8_t(std::size_t)> read_fn;
  std::function<void(std::size_t, std::uint8_t)> write_fn;

  MemoryRegionMMIO(std::function<std::uint8_t(std::size_t)> read_fn,
                   std::function<void(std::size_t, std::uint8_t)> write_fn)
      : read_fn(read_fn), write_fn(write_fn) {}

  std::size_t get_size() const override { return 0; }
  std::uint8_t read(std::size_t offset) const override { return read_fn(offset); }
  void write(std::size_t offset, std::uint8_t value) override { write_fn(offset, value); }
};

} // namespace MultiEmu
