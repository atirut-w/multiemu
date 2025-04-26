#pragma once
#include <functional>
#include <iostream>
#include <vector>

namespace MultiEmu {

template <typename AT, typename DT> class Bus {
public:
  struct Region {
    AT start, end;
    std::function<DT(AT)> read;
    std::function<void(AT, DT)> write;
    int priority = 0;
  };

  void mapRegion(AT base, int size, std::function<DT(AT)> read, std::function<void(AT, DT)> write, int priority = 0) {
    regions.push_back({base, (AT)(base + size - 1), read, write, priority});
  }

  DT read(AT address) {
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

  void write(AT address, DT value) {
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

private:
  std::vector<Region> regions;
};

} // namespace MultiEmu
