#include <z80/z80.hpp>

uint8_t Z80::fetch()
{
    return read(pc++);
}

void Z80::run_instruction()
{
    uint8_t opcode = fetch();
    uint8_t x = (opcode & 0b11000000) >> 6;
    uint8_t y = (opcode & 0b00111000) >> 3;
    uint8_t z = opcode & 0b00000111;
    uint8_t p = (y & 0b110) >> 1;
    uint8_t q = y & 0b001;
}
