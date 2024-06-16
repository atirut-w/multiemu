#include <6502/6502.hpp>

void MOS6502::run_instruction()
{
    if (resetting)
    {
        pc = read(0xfffc) | (read(0xfffd) << 8);
        resetting = false;
    }

    uint8_t opcode = read(pc++);
    uint8_t opcode_hi = opcode >> 4;
    uint8_t opcode_lo = opcode & 0xf;

    if (opcode_lo == 8)
    {
        // TODO: SB1
    }
    else if (opcode_lo == 0xa && opcode_hi > 7)
    {
        // TODO: SB2
    }
    else
    {
        uint8_t aaa = (opcode & 0xe0) >> 5;
        uint8_t bbb = (opcode & 0x1c) >> 2;
        uint8_t cc = opcode & 0x3;

        switch (cc)
        {
        case 1:
            // TODO: Group 1
            break;
        case 2:
            // TODO: Group 2
            break;
        case 3:
            if (bbb == 4)
            {
                // TODO: Conditional branch
            }
            else if (bbb == 0 && !(aaa & 4))
            {
                // TODO: I/S
            }
            else
            {
                // TODO: Group 3
            }
            break;
        }
    }
}
