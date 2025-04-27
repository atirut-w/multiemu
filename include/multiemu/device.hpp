#pragma once

namespace MultiEmu {

class Device {
public:
  virtual ~Device() = default;
  
  // Core interface
  virtual void reset() = 0;
  virtual void tick(int cycles) = 0;
};

} // namespace MultiEmu