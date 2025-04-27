#pragma once
#include "multiemu/device.hpp"
#include <memory>
#include <functional>
#include <cstdint>

namespace MultiEmu {

class Z80;

class Z80PIO : public Device {
private:
  // Opaque pointer to implementation
  struct Impl;
  std::unique_ptr<Impl> pImpl;

public:
  Z80PIO(Z80* cpu);
  virtual ~Z80PIO();

  // Prevent copying
  Z80PIO(const Z80PIO&) = delete;
  Z80PIO& operator=(const Z80PIO&) = delete;

  // Allow moving
  Z80PIO(Z80PIO&&) noexcept;
  Z80PIO& operator=(Z80PIO&&) noexcept;

  // Device interface
  virtual void reset() override;
  virtual void tick(int cycles) override;

  // Pin access for direct connection to CPU and other peripherals
  uint64_t getPins() const;
  void setPins(uint64_t pins);
  
  // Low-level tick function for direct pin control
  uint64_t tick(uint64_t pins);
  
  // Port A/B control
  void setPortAInput(uint8_t value);
  void setPortBInput(uint8_t value);
  uint8_t getPortAOutput() const;
  uint8_t getPortBOutput() const;
  
  // IO port mappings (for traditional I/O port access)
  uint8_t readData(uint8_t port);
  void writeData(uint8_t port, uint8_t value);
  uint8_t readControl(uint8_t port);
  void writeControl(uint8_t port, uint8_t value);
  
  // Callback types for port events
  using PortCallback = std::function<void(uint8_t)>;
  
  // Set callbacks for port output changes
  void setPortACallback(PortCallback callback);
  void setPortBCallback(PortCallback callback);
};

} // namespace MultiEmu