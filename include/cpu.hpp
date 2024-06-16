#pragma once
#include <cstdint>
#include <functional>

class CPU
{
public:
    virtual ~CPU() = default; // Force polymorphism

    std::function<uint8_t(uint64_t)> read;
    std::function<void(uint64_t, uint8_t)> write;

    virtual void run_instruction() = 0;
};
