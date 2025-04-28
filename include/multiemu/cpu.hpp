#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <vector>
#include "multiemu/device.hpp"

namespace MultiEmu {

// CPU debug-related structures
struct RegisterInfo {
  std::string name;
  uint8_t bitWidth;
  std::string displayFormat = "hex"; // hex, dec, bin
  std::string description;
  bool isReadOnly = false;
};

struct FlagDefinition {
  std::string name;
  char shortName;
  uint8_t bitPosition;
  std::string description;
};

struct RegisterGroup {
  std::string name;
  std::vector<std::string> registerNames;
};

struct DebuggerCapabilities {
  bool hasStatusRegister = false;
  std::string statusRegisterName;
};

/**
 * CPU base class - inherits from Device
 */
class CPU : public Device {
public:
  using ReadCallback = std::function<uint8_t(size_t)>;
  using WriteCallback = std::function<void(size_t, uint8_t)>;

  ReadCallback read = [](size_t) { return 0; };
  WriteCallback write = [](size_t, uint8_t) {};
  ReadCallback in = [](size_t) { return 0; };
  WriteCallback out = [](size_t, uint8_t) {};

  virtual ~CPU() = default;

  // Device interface implementation
  virtual std::string getDeviceType() const override { return "cpu"; }
  virtual void initialize() override { reset(); }
  
  // Core execution (inherited from Device, needs to be re-declared here)
  virtual int execute(int cycles) override = 0;
  virtual void reset() override = 0;
  
  // Register/debugging metadata
  virtual DebuggerCapabilities getDebuggerCapabilities() const {
    return DebuggerCapabilities{};
  }
  
  virtual std::vector<RegisterInfo> getRegisterInfo() const {
    return {}; // Override in specific CPUs
  }
  
  virtual std::vector<RegisterGroup> getRegisterGroups() const {
    return {}; // Override in specific CPUs
  }
  
  virtual std::vector<FlagDefinition> getFlagDefinitions() const {
    return {}; // Override in specific CPUs
  }
  
  // Register access
  virtual uint64_t getRegister(const std::string& name) = 0;
  virtual bool setRegister(const std::string& name, uint64_t value) = 0;
  virtual std::map<std::string, uint64_t> getAllRegisters() = 0;
  
  // Program counter access for memory view positioning
  virtual size_t getProgramCounter() const = 0;
  
  // Interrupt interface
  virtual void requestInterrupt(uint32_t vector = 0, bool nmi = false) { /* Default no-op */ }
  
  // For debugging/status purposes
  virtual bool areInterruptsEnabled() const { return false; /* Default: interrupts disabled */ }
};

} // namespace MultiEmu
