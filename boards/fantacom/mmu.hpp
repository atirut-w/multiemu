#pragma once
#include "multiemu/bus_device.hpp"
#include <array>
#include <cstdint>

struct MMU : MultiEmu::BusDevice<uint8_t> {
  std::array<std::uint8_t, 16> pagemap;

  std::uint8_t read(std::size_t addr) override;
  void write(std::size_t addr, std::uint8_t data) override;
  std::uint32_t translate(std::uint32_t addr);
};
