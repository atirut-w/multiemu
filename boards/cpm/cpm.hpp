#pragma once
#include "multiemu/board.hpp"
#include "z80/z80.hpp"

class CPMBoard : public Board {
  Z80 cpu;

  virtual int step() override;
};
