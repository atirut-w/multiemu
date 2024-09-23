#pragma once
#include "multiemu/cpu.hpp"
#include "multiemu/io.hpp"

class Z80 : public CPU, public IO {
  void *real_cpu;

public:
  Z80();
  ~Z80();
  int step() override;
};
