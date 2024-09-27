#pragma once
#include <cstddef>

template <typename DataType> struct BusDevice {
  std::size_t offset = 0;
  virtual DataType read(std::size_t addr) { return 0; }
  virtual void write(std::size_t addr, DataType data) {}
};
