#include "multiemu/scheduler.hpp"
#include <numeric>

using namespace MultiEmu;

void Scheduler::schedule(Callback callback, int interval) {
  events.push_back({callback, interval});
  computeQuantum();
}

int Scheduler::run(int cycles) {
  int total = 0;

  while (total < cycles && !events.empty()) {
    for (auto& event : events) {
      event.accumulator += quantum;
      while (event.accumulator >= event.interval) {
        int ran = event.callback(event.accumulator);
        total += ran;
        event.accumulator -= ran;

        if (total >= cycles) {
          break;
        }
      }
    }
  }

  return total;
}

void Scheduler::computeQuantum() {
  if (events.empty()) {
    quantum = 0;  // No events, no quantum
    return;
  }

  quantum = events[0].interval;
  for (const auto& event : events) {
    quantum = std::lcm(quantum, event.interval);
  }
}
