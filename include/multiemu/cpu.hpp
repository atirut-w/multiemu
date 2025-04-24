#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>

namespace MultiEmu {

class CPU {
public:
  using ReadCallback = std::function<uint8_t(size_t)>;
  using WriteCallback = std::function<void(size_t, uint8_t)>;

  ReadCallback read = [](size_t) { return 0; };
  WriteCallback write = [](size_t, uint8_t) {};
  ReadCallback in = [](size_t) { return 0; };
  WriteCallback out = [](size_t, uint8_t) {};

  virtual ~CPU() = default;

  virtual int execute(int cycles) = 0;
};

} // namespace MultiEmu
