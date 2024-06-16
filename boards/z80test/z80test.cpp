#include <z80test/z80test.hpp>
#include <z80/z80.hpp>
#include <iostream>

using namespace std;

Z80Test::Z80Test()
{
    cpu = std::make_unique<Z80>();
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
