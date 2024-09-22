#pragma once
#include "multiemu/board.hpp"

struct CPMBoard : Board {
  virtual int step() override;
};
