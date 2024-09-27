#pragma once
#include "multiemu/bus.hpp"
#include "multiemu/bus_device.hpp"
#include "raylib.h"
#include <array>
#include <cstdint>
#include <optional>

struct Graphics : public MultiEmu::BusDevice<uint8_t> {
  std::array<uint8_t, 16> data;
  MultiEmu::Bus<uint8_t> *rambus;
  std::optional<RenderTexture2D> charset;
  bool dirty = true;

  virtual uint8_t read(std::size_t addr) override;
  virtual void write(std::size_t addr, uint8_t data) override;

  void refresh_charset();
  void draw();

  uint32_t get_address(int index) {
    return data[index] << 16 | data[index + 1] << 8 | data[index + 2];
  }
};
