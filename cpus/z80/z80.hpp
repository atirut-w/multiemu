#pragma once
#include "multiemu/cpu.hpp"
#include "multiemu/io.hpp"

class Z80 : public CPU, public IO {
public:
  virtual int step() override;
};
