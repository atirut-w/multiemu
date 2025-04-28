#pragma once

#include <memory>
#include <string>
#include <vector>

namespace MultiEmu {

/**
 * Base class for all emulation devices
 * Establishes a device hierarchy similar to MAME's device system
 */
class Device {
public:
  virtual ~Device() = default;

  // Basic device lifecycle
  virtual void initialize() = 0;
  virtual void reset() = 0;
  
  // Device identification
  virtual std::string getDeviceType() const = 0;
  virtual std::string getDeviceName() const = 0;
  
  // Device hierarchy
  void addChild(std::unique_ptr<Device> device);
  const std::vector<std::unique_ptr<Device>>& getChildren() const;
  
  // Make class movable but not copyable
  Device() = default;
  Device(const Device&) = delete;
  Device& operator=(const Device&) = delete;
  Device(Device&&) = default;
  Device& operator=(Device&&) = default;
  
  // Device ownership
  Device* getParent() const;
  void setParent(Device* parent);
  
  // Clock-related functionality
  virtual int execute(int cycles) { return 0; }
  
protected:
  std::vector<std::unique_ptr<Device>> children;
  Device* parent = nullptr;
};

} // namespace MultiEmu
