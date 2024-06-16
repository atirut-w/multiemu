#pragma once
#include <cpu.hpp>
#include <cstdint>

struct MOS6502 : public CPU
{
    uint8_t a, x, y, stack, status;
    uint16_t pc;
    bool resetting = true;

    void run_instruction() override;
};
