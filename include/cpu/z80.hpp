#include "multiemu/cpu.hpp"
#include <memory>

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
  Z80(const Z80&) = delete;
  Z80& operator=(const Z80&) = delete;
  
  // Allow moving
  Z80(Z80&&) noexcept;
  Z80& operator=(Z80&&) noexcept;

  virtual int execute(int cycles) override;
};

}
