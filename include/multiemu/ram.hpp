#pragma once
#include "multiemu/bus_device.hpp"
#include <cstdint>
#include <vector>

namespace MultiEmu {
struct RAM : BusDevice<uint8_t> {
  RAM() = default;
  RAM(std::size_t size) : data(size) {}

  std::vector<uint8_t> data;
  uint8_t read(std::size_t addr) override {
    return addr < data.size() ? data[addr] : 0;
  }
  void write(std::size_t addr, uint8_t value) override {
    if (addr < data.size()) {
      data[addr] = value;
    }
  }
};
} // namespace MultiEmu
