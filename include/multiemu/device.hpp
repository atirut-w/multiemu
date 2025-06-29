#pragma once

namespace MultiEmu {

struct Device {
  virtual int run(int cycles) = 0;
};

} // namespace MultiEmu
