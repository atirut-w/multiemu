#include "devices/z80pio.hpp"
#include "cpu/z80.hpp"
#include <stdexcept>
#include "chips/z80.h"  // Include for Z80 pin definitions

#define CHIPS_IMPL
#include "chips/z80pio.h"

namespace MultiEmu {

// Define the implementation struct
struct Z80PIO::Impl {
  z80pio_t pio;
  uint64_t pins;
  Z80* cpu;  // Non-owning pointer - just a reference
  
  // Port callbacks
  PortCallback portACallback;
  PortCallback portBCallback;
  
  // Cached port values
  uint8_t portAOutput;
  uint8_t portBOutput;
  
  Impl(Z80* cpu) : cpu(cpu) {
    // Initialize the Z80 PIO
    z80pio_init(&pio);
    pins = 0;
    
    // Initialize cached port values
    portAOutput = 0;
    portBOutput = 0;
  }
  
  // Process direct connections to CPU pins
  void updateCPUPins() {
    if (cpu) {
      // Get current CPU pins
      uint64_t cpuPins = cpu->getPins();
      
      // Merge PIO interrupt line with CPU pins
      if (pins & Z80PIO_INT) {
        cpuPins |= Z80_INT;
      }
      
      // Set the updated CPU pins
      cpu->setPins(cpuPins);
    }
  }
  
  // Update the cached port output values
  void updatePortOutputs() {
    uint8_t newPortA = Z80PIO_GET_PA(pins);
    uint8_t newPortB = Z80PIO_GET_PB(pins);
    
    // If port values have changed, trigger callbacks
    if (newPortA != portAOutput) {
      portAOutput = newPortA;
      if (portACallback) {
        portACallback(portAOutput);
      }
    }
    
    if (newPortB != portBOutput) {
      portBOutput = newPortB;
      if (portBCallback) {
        portBCallback(portBOutput);
      }
    }
  }
};

// Constructor - cpu is just a reference, not owned by PIO
Z80PIO::Z80PIO(Z80* cpu) : pImpl(std::make_unique<Impl>(cpu)) {
  // Constructor implementation
}

// Destructor 
Z80PIO::~Z80PIO() = default;

// Move constructor
Z80PIO::Z80PIO(Z80PIO&&) noexcept = default;

// Move assignment
Z80PIO& Z80PIO::operator=(Z80PIO&&) noexcept = default;

// Reset the PIO
void Z80PIO::reset() {
  z80pio_reset(&pImpl->pio);
  pImpl->pins = 0;
}

// Tick the PIO using default approach
void Z80PIO::tick(int cycles) {
  // Process the PIO for the specified number of cycles
  for (int i = 0; i < cycles; i++) {
    if (pImpl->cpu) {
      // Get the CPU pins
      uint64_t cpuPins = pImpl->cpu->getPins();
      
      // Copy shared pins from CPU to PIO
      // Only preserve PIO-specific pins and copy CPU-relevant pins
      pImpl->pins &= ~((1ULL<<40)-1);  // Clear CPU-related pins (bits 0-39)
      pImpl->pins |= (cpuPins & ~Z80_INT);  // Copy CPU pins except INT (PIO manages its own INT)
      
      // Let the PIO process the pins
      pImpl->pins = z80pio_tick(&pImpl->pio, pImpl->pins);
      
      // Update cached port outputs
      pImpl->updatePortOutputs();
      
      // Update CPU pins with PIO signals (like INT)
      pImpl->updateCPUPins();
    } else {
      // No CPU connected, just tick the PIO
      pImpl->pins = z80pio_tick(&pImpl->pio, pImpl->pins);
      pImpl->updatePortOutputs();
    }
  }
}

// Tick with explicit pin control
uint64_t Z80PIO::tick(uint64_t pins) {
  // Set the input pins
  pImpl->pins = pins;
  
  // Tick the PIO
  pImpl->pins = z80pio_tick(&pImpl->pio, pImpl->pins);
  
  // Update cached port outputs
  pImpl->updatePortOutputs();
  
  // Return the updated pins
  return pImpl->pins;
}

// Pin access for direct peripheral connections
uint64_t Z80PIO::getPins() const {
  return pImpl->pins;
}

void Z80PIO::setPins(uint64_t pins) {
  pImpl->pins = pins;
}

// Set Port A input value
void Z80PIO::setPortAInput(uint8_t value) {
  // Set the Port A inputs on the pins
  Z80PIO_SET_PA(pImpl->pins, value);
}

// Set Port B input value
void Z80PIO::setPortBInput(uint8_t value) {
  // Set the Port B inputs on the pins
  Z80PIO_SET_PB(pImpl->pins, value);
}

// Get Port A output value
uint8_t Z80PIO::getPortAOutput() const {
  return pImpl->portAOutput;
}

// Get Port B output value
uint8_t Z80PIO::getPortBOutput() const {
  return pImpl->portBOutput;
}

// Read from PIO data register
uint8_t Z80PIO::readData(uint8_t port) {
  // Setup pins for data read operation
  uint64_t tempPins = Z80PIO_CE | Z80PIO_IORQ | Z80PIO_RD;
  
  // Select port A or B
  if (port == 1) {
    tempPins |= Z80PIO_BASEL;  // Port B
  }
  
  // Perform the read
  tempPins = z80pio_tick(&pImpl->pio, tempPins);
  
  // Extract and return the data
  return Z80PIO_GET_DATA(tempPins);
}

// Write to PIO data register
void Z80PIO::writeData(uint8_t port, uint8_t value) {
  // Setup pins for data write operation
  uint64_t tempPins = Z80PIO_CE | Z80PIO_IORQ;
  
  // Select port A or B
  if (port == 1) {
    tempPins |= Z80PIO_BASEL;  // Port B
  }
  
  // Set the data value
  Z80PIO_SET_DATA(tempPins, value);
  
  // Perform the write
  tempPins = z80pio_tick(&pImpl->pio, tempPins);
  
  // Update our pins with changes from the PIO
  pImpl->pins = (pImpl->pins & ~Z80PIO_INT) | (tempPins & Z80PIO_INT);
  
  // Update port outputs
  pImpl->updatePortOutputs();
}

// Read from PIO control register
uint8_t Z80PIO::readControl(uint8_t port) {
  // Setup pins for control read operation
  uint64_t tempPins = Z80PIO_CE | Z80PIO_IORQ | Z80PIO_RD | Z80PIO_CDSEL;
  
  // Select port A or B
  if (port == 1) {
    tempPins |= Z80PIO_BASEL;  // Port B
  }
  
  // Perform the read
  tempPins = z80pio_tick(&pImpl->pio, tempPins);
  
  // Extract and return the data
  return Z80PIO_GET_DATA(tempPins);
}

// Write to PIO control register
void Z80PIO::writeControl(uint8_t port, uint8_t value) {
  // Setup pins for control write operation
  uint64_t tempPins = Z80PIO_CE | Z80PIO_IORQ | Z80PIO_CDSEL;
  
  // Select port A or B
  if (port == 1) {
    tempPins |= Z80PIO_BASEL;  // Port B
  }
  
  // Set the data value
  Z80PIO_SET_DATA(tempPins, value);
  
  // Perform the write
  tempPins = z80pio_tick(&pImpl->pio, tempPins);
  
  // Update our pins with changes from the PIO
  pImpl->pins = (pImpl->pins & ~Z80PIO_INT) | (tempPins & Z80PIO_INT);
  
  // Update port outputs
  pImpl->updatePortOutputs();
}

// Set callback for Port A changes
void Z80PIO::setPortACallback(PortCallback callback) {
  pImpl->portACallback = callback;
}

// Set callback for Port B changes
void Z80PIO::setPortBCallback(PortCallback callback) {
  pImpl->portBCallback = callback;
}

} // namespace MultiEmu
