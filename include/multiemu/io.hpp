#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>

namespace MultiEmu {
struct IO {
  std::function<uint8_t(size_t)> in = [](size_t) { return 0; };
  std::function<void(size_t, uint8_t)> out = [](size_t, uint8_t) {};
};
} // namespace MultiEmu
