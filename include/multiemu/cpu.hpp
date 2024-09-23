#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>

using ReadCallback = std::function<uint8_t(size_t)>;
using WriteCallback = std::function<void(size_t, uint8_t)>;

struct CPU {
  ReadCallback fetch_opcode = [](size_t) { return 0; };
  ReadCallback read = [](size_t) { return 0; };
  WriteCallback write = [](size_t, uint8_t) {};

  virtual int run(int cycles) = 0;
  virtual void set_trap(size_t opcode, std::function<size_t(size_t)> handler) {}
};
