#include "device.hpp"
#include <functional>
#include <vector>

namespace MultiEmu {

struct Scheduler : public Device {
  using Callback = std::function<int(int)>;

  void schedule(Callback callback, int interval);
  virtual int run(int cycles) override;

private:
  struct ScheduledEvent {
    Callback callback;
    int interval;
    int accumulator = 0;
  };

  std::vector<ScheduledEvent> events;
  int quantum;

  void computeQuantum();
};

} // namespace MultiEmu
