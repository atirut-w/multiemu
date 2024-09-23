#pragma once
#include "multiemu/cpu.hpp"
#include "multiemu/io.hpp"
#include <cstddef>
#include <cstdint>
#include <map>

class Z80 : public CPU, public IO {

public:
  Z80();
  ~Z80();

  void *real_cpu;
  std::map<size_t, std::function<size_t(size_t)>> traps;

  int run(int cycles) override;
  virtual void set_trap(size_t opcode,
                        std::function<size_t(size_t)> handler) override;
};
