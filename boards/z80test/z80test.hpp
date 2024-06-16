#pragma once
#include <board.hpp>

struct Z80Test : public Board
{
    Z80Test();
};

std::unique_ptr<Board> zx48k_ctor()
{
    return std::make_unique<Z80Test>();
}

REGISTER_BOARD(zx48k, zx48k_ctor)
