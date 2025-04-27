#include "multiemu/cpu.hpp"
#include <memory>
#include <functional>
#include <cstdint>

namespace MultiEmu {

class Z80 : public CPU {
private:
  // Opaque pointer to implementation
  struct Impl;
  std::unique_ptr<Impl> pImpl;

public:
  Z80();
  virtual ~Z80();

  // Prevent copying
  Z80(const Z80 &) = delete;
  Z80 &operator=(const Z80 &) = delete;

  // Allow moving
  Z80(Z80 &&) noexcept;
  Z80 &operator=(Z80 &&) noexcept;

  // Core execution
  virtual int execute(int cycles) override;
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
  
  // Pin access for direct connection to peripherals
  uint64_t getPins() const;
  void setPins(uint64_t pins);
  
  // Tick function that returns new pin state
  uint64_t tick(uint64_t pins);
};

} // namespace MultiEmu