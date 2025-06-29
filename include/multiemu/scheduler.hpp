#include "device.hpp"
#include <functional>
#include <vector>

namespace MultiEmu {

struct Scheduler : public Device {
  using Callback = std::function<int(int)>;

  void schedule(Callback callback, int frequency);
  virtual int run(int cycles) override;

private:
  struct ScheduledEvent {
    Callback callback;
    int frequency;
    int accumulator = 0;
  };

  std::vector<ScheduledEvent> events;
  int quantum_cycles;

  void computeQuantum();
};

} // namespace MultiEmu
