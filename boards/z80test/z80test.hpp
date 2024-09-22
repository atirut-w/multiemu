#pragma once
#include <board.hpp>

struct Z80Test : public Board {
  Z80Test();
};

std::unique_ptr<Board> z80test_ctor_wrap() {
  return std::make_unique<Z80Test>();
}

REGISTER_BOARD(z80test, z80test_ctor_wrap)
