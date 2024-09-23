#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>

struct CPU {
  virtual int step() = 0;
  std::function<uint8_t(size_t)> read;
  std::function<void(size_t, uint8_t)> write;
};
