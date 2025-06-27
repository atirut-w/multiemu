#include "multiemu/cpu.hpp"
#include <cstdint>
#include <memory>
#include <z80.h>

namespace MultiEmu {

class Z80 : public CPU {
private:
  z80_t cpu;
  std::uint64_t pins = 0;

public:
  Z80();
  
  ReadCallback in = [](size_t) { return 0; };
  WriteCallback out = [](size_t, uint8_t) {};

  // Core execution
  virtual int run(int cycles) override;
  virtual void reset() override;

  // Debugging capability and metadata
  virtual DebuggerCapabilities getDebuggerCapabilities() const override;
  virtual std::vector<RegisterInfo> getRegisterInfo() const override;
  virtual std::vector<RegisterGroup> getRegisterGroups() const override;
  virtual std::vector<FlagDefinition> getFlagDefinitions() const override;

  // Register access
  virtual uint64_t getRegister(const std::string &name) override;
  virtual bool setRegister(const std::string &name, uint64_t value) override;
  virtual std::map<std::string, uint64_t> getAllRegisters() override;

  // Program counter access for memory view positioning
  virtual size_t getProgramCounter() const override;

  // Interrupt interface
  virtual void requestInterrupt(uint32_t vector = 0, bool nmi = false) override;
  virtual bool areInterruptsEnabled() const override;
};

} // namespace MultiEmu
