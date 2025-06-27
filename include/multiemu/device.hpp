#pragma once

namespace MultiEmu {

class Device {
public:
  virtual ~Device() = default;

  virtual int run(int cycles) = 0;
};

} // namespace MultiEmu
