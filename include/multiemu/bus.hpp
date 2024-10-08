#pragma once
#include "multiemu/bus_device.hpp"
#include <algorithm>
#include <vector>

namespace MultiEmu {
template <typename DataType> class Bus {
  DataType data;
  std::vector<BusDevice<DataType> *> listeners;

public:
  void add_listener(BusDevice<DataType> *listener) {
    if (std::find(listeners.begin(), listeners.end(), listener) ==
        listeners.end()) {
      listeners.push_back(listener);
    }
  }

  void remove_listener(BusDevice<DataType> *listener) {
    auto it = std::find(listeners.begin(), listeners.end(), listener);
    if (it != listeners.end()) {
      listeners.erase(it);
    }
  }

  DataType read(std::size_t addr) {
    DataType data = 0;
    for (auto *listener : listeners) {
      if (addr >= listener->offset) {
        // We use bitwise OR for happy accidents where multiple devices try to
        // drive the bus at the same time.
        data |= listener->read(addr - listener->offset);
      }
    }

    return data;
  }

  void write(std::size_t addr, DataType data) {
    this->data = data;
    for (auto *listener : listeners) {
      if (addr >= listener->offset) {
        listener->write(addr - listener->offset, data);
      }
    }
  }

  DataType get_data() const { return data; }
};
} // namespace MultiEmu
