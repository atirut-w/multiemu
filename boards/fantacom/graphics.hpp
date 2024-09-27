#pragma once
#include "multiemu/bus_device.hpp"
#include <array>
#include <cstdint>

struct Graphics : public BusDevice<uint8_t> {
  std::array<uint8_t, 16> data;

  virtual uint8_t read(std::size_t addr) override;
  virtual void write(std::size_t addr, uint8_t data) override;
  void draw();

  uint32_t get_address(int index) {
    return data[index] << 16 | data[index + 1] << 8 | data[index + 2];
  }
};