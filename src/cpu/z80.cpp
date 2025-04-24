#include "cpu/z80.hpp"
#include <z80.hpp>

namespace MultiEmu {

// Define the implementation struct
struct Z80::Impl {
    ::Z80 cpu;
    
    Impl(Z80* parent) {
        // Set up callbacks, capturing the parent Z80 class pointer
        cpu.setupCallback(
            [parent](void*, uint16_t address) { return parent->read(address); },
            [parent](void*, uint16_t address, uint8_t value) { parent->write(address, value); },
            [parent](void*, uint16_t address) { return parent->in(address); },
            [parent](void*, uint16_t address, uint8_t value) { parent->out(address, value); }
        );
    }
};

// Constructor - initialize implementation
Z80::Z80() : pImpl(std::make_unique<Impl>(this)) {}

// Destructor - needed for unique_ptr with incomplete type
Z80::~Z80() = default;

// Move constructor
Z80::Z80(Z80&&) noexcept = default;

// Move assignment
Z80& Z80::operator=(Z80&&) noexcept = default;

// Execute method - delegate to implementation
int Z80::execute(int cycles) {
    return pImpl->cpu.execute(cycles);
}

} // namespace MultiEmu
