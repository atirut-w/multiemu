#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>

struct CPU {
  std::function<uint8_t(size_t)> read = [](size_t) { return 0; };
  std::function<void(size_t, uint8_t)> write = [](size_t, uint8_t) {};

  virtual int step() = 0;
  virtual void set_trap(size_t opcode, std::function<size_t(size_t)> handler) {}
};
