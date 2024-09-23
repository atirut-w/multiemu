#pragma once
#include "multiemu/cpu.hpp"
#include <memory>

class Z80 : public CPU {
  void *real_cpu;

public:
  Z80();
  ~Z80();
  int step() override;
};
