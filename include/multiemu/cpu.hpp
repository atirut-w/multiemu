#pragma once
#include "multiemu/io.hpp"
#include <cstddef>
#include <cstdint>
#include <functional>

struct CPU : public IO {
  virtual int step() = 0;
  std::function<uint8_t(size_t)> read = [](size_t) { return 0; };
  std::function<void(size_t, uint8_t)> write = [](size_t, uint8_t) {};
};
