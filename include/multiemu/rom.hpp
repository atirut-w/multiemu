#pragma once
#include "multiemu/bus_device.hpp"
#include <cstdint>
#include <vector>

struct ROM : BusDevice<uint8_t> {
  ROM() = default;
  ROM(const std::vector<uint8_t> &data) : data(data) {}

  std::vector<uint8_t> data;
  uint8_t read(std::size_t addr) override {
    return addr < data.size() ? data[addr] : 0;
  }
};
