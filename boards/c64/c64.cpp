#include <c64/c64.hpp>
#include <iostream>
#include <6502/6502.hpp>

using namespace std;

C64::C64()
{
    cpu = make_unique<MOS6502>();
    cpu->read = [this](uint16_t addr) -> uint8_t
    {
        cout << "Reading from " << hex << addr << endl;
        return 0;
    };
    cpu->write = [this](uint16_t addr, uint8_t data)
    {
        cout << "Writing " << hex << static_cast<int>(data) << " to " << hex << addr << endl;
    };
}
