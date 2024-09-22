#pragma once
#include <cstddef>

template <typename DataWype> struct BusDevice {
  virtual DataWype read(std::size_t addr) { return 0; }
  virtual void write(std::size_t addr, DataWype data) {}
};
