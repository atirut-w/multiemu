#include <z80/z80.hpp>

uint8_t Z80::fetch()
{
    return read(pc++);
}

void Z80::run_instruction()
{
    uint8_t opcode = fetch();
}
