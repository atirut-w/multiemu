#pragma once
#include <cstddef>

template <typename DataType> struct BusDevice {
  virtual DataType read(std::size_t addr) { return 0; }
  virtual void write(std::size_t addr, DataType data) {}
};
