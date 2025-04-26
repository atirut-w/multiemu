#pragma once
#include <functional>
#include <iostream>
#include <vector>
#include <cstdint>

namespace MultiEmu {

/**
 * Non-templated Bus class with fixed types but variable bit width
 * This version simplifies debugger integration at a small cost to type safety
 */
class Bus {
public:
  enum class Width {
    BUS_8BIT,   // 8-bit address bus
    BUS_16BIT,  // 16-bit address bus
    BUS_32BIT   // 32-bit address bus
  };

  struct Region {
    uint32_t start, end;
    std::function<uint8_t(uint32_t)> read;
    std::function<void(uint32_t, uint8_t)> write;
    int priority = 0;
  };

  // Constructor requires address width information
  Bus(Width addressWidth = Width::BUS_16BIT) : addressWidth(addressWidth) {}

  // Map a memory region to this bus
  void mapRegion(uint32_t base, int size, 
                std::function<uint8_t(uint32_t)> read, 
                std::function<void(uint32_t, uint8_t)> write, 
                int priority = 0) {
    regions.push_back({base, base + size - 1, read, write, priority});
  }

  // Convenience overloads for 8-bit address
  void mapRegion8(uint8_t base, int size, 
                 std::function<uint8_t(uint8_t)> read, 
                 std::function<void(uint8_t, uint8_t)> write, 
                 int priority = 0) {
    // Wrap 8-bit callbacks to 32-bit
    auto read32 = [read](uint32_t addr) -> uint8_t { 
      return read(static_cast<uint8_t>(addr)); 
    };
    auto write32 = [write](uint32_t addr, uint8_t val) { 
      write(static_cast<uint8_t>(addr), val); 
    };
    mapRegion(base, size, read32, write32, priority);
  }

  // Convenience overloads for 16-bit address
  void mapRegion16(uint16_t base, int size, 
                  std::function<uint8_t(uint16_t)> read, 
                  std::function<void(uint16_t, uint8_t)> write, 
                  int priority = 0) {
    // Wrap 16-bit callbacks to 32-bit
    auto read32 = [read](uint32_t addr) -> uint8_t { 
      return read(static_cast<uint16_t>(addr)); 
    };
    auto write32 = [write](uint32_t addr, uint8_t val) { 
      write(static_cast<uint16_t>(addr), val); 
    };
    mapRegion(base, size, read32, write32, priority);
  }

  // Read from the bus (all address widths)
  uint8_t read(uint32_t address) {
    Region *best = nullptr;
    for (auto &region : regions) {
      if (address >= region.start && address <= region.end) {
        if (!best || region.priority > best->priority) {
          best = &region;
        }
      }
    }
    if (best) {
      return best->read(address);
    }
    std::cerr << "Bus read out of range @ " << std::hex << address << std::dec << "\n";
    return 0; // Return a default value for out-of-range reads
  }

  // Convenience overloads for different address widths
  uint8_t read8(uint8_t address) {
    return read(static_cast<uint32_t>(address));
  }

  uint8_t read16(uint16_t address) {
    return read(static_cast<uint32_t>(address));
  }

  // Write to the bus (all address widths)
  void write(uint32_t address, uint8_t value) {
    Region *best = nullptr;
    for (auto &region : regions) {
      if (address >= region.start && address <= region.end) {
        if (!best || region.priority > best->priority) {
          best = &region;
        }
      }
    }
    if (best) {
      best->write(address, value);
    } else {
      std::cerr << "Bus write out of range @ " << std::hex << address << std::dec << "\n";
    }
  }

  // Convenience overloads for different address widths
  void write8(uint8_t address, uint8_t value) {
    write(static_cast<uint32_t>(address), value);
  }

  void write16(uint16_t address, uint8_t value) {
    write(static_cast<uint32_t>(address), value);
  }

  // Get the address width of this bus
  Width getAddressWidth() const {
    return addressWidth;
  }

  // Get the maximum possible address for this bus
  uint32_t getMaxAddress() const {
    switch (addressWidth) {
      case Width::BUS_8BIT:
        return 0xFF;
      case Width::BUS_16BIT:
        return 0xFFFF;
      case Width::BUS_32BIT:
      default:
        return 0xFFFFFFFF;
    }
  }

private:
  std::vector<Region> regions;
  Width addressWidth;
};

} // namespace MultiEmu