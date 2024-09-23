#pragma once
#include "multiemu/cpu.hpp"
#include "multiemu/io.hpp"
#include <cstddef>
#include <cstdint>
#include <map>

class Z80 : public CPU, public IO {
  void *real_cpu;

public:
  Z80();
  ~Z80();

  std::map<size_t, std::function<size_t(size_t)>> traps;

  int step() override;
  virtual void set_trap(size_t opcode,
                        std::function<size_t(size_t)> handler) override;
};
