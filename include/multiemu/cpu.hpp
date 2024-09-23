#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>

using ReadCallback = std::function<uint8_t(size_t)>;
using WriteCallback = std::function<void(size_t, uint8_t)>;

struct CPU {
  ReadCallback read = [](size_t) { return 0; };
  WriteCallback write = [](size_t, uint8_t) {};

  virtual int step() = 0;
};
