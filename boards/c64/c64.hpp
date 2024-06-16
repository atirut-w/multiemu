#pragma once
#include <board.hpp>

struct C64 : Board
{
};

std::unique_ptr<Board> c64_ctor_wrap()
{
    return std::make_unique<C64>();
}

REGISTER_BOARD(c64, c64_ctor_wrap)
