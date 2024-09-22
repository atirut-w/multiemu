#pragma once
#include <cpu.hpp>
#include <cstdint>

struct Z80 : public CPU {
  uint16_t pc = 0;

  uint8_t fetch();
  void run_instruction() override;
};
