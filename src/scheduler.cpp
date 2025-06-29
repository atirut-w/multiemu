#include "multiemu/scheduler.hpp"
#include <numeric>

using namespace MultiEmu;

void Scheduler::schedule(Callback callback, int frequency) {
  events.push_back({callback, frequency});
  computeQuantum();
}

int Scheduler::run(int cycles) {
  int cycles_remaining = cycles;

  while (cycles_remaining > 0) {
    int slice = std::min(quantum_cycles, cycles_remaining);
    cycles_remaining -= slice;

    for (auto& event : events) {
      event.accumulator += slice;
      int interval = quantum_cycles / event.frequency;
      while (event.accumulator >= interval) {
        event.accumulator -= event.callback(slice);
      }
    }
  }

  return cycles - cycles_remaining;
}

void Scheduler::computeQuantum() {
  if (events.empty()) {
    quantum_cycles = 0;  // No events, no quantum
    return;
  }

  quantum_cycles = events[0].frequency;
  for (const auto& event : events) {
    quantum_cycles = std::lcm(quantum_cycles, event.frequency);
  }
}
