#include <c64/c64.hpp>
#include <iostream>
#include <6502/6502.hpp>

using namespace std;

C64::C64()
{
    cpu = make_unique<MOS6502>();
}
