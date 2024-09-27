#pragma once
#include "multiemu/device.hpp"
#include <cstddef>

namespace MultiEmu {
template <typename DataType> struct BusDevice : public Device {
  std::size_t offset = 0;
  virtual DataType read(std::size_t addr) { return 0; }
  virtual void write(std::size_t addr, DataType data) {}
};
} // namespace MuliEmu
