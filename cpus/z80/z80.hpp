#pragma once
#include "multiemu/cpu.hpp"

class Z80 : public CPU {
public:
  int step() override;
};
