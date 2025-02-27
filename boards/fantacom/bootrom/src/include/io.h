#pragma once
#include <stdint.h>

uint8_t in(uint16_t address);

void out(uint16_t address, uint8_t value);
void out16(uint16_t address, uint16_t value);
void out32(uint16_t address, uint32_t value);
