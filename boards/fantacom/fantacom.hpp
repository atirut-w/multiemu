#pragma once
#include "multiemu/board.hpp"

class FantacomBoard : public Board {
public:
  FantacomBoard();

  std::array<uint8_t, 0x4000> rom;

  virtual int run(int cycles) override;
};
