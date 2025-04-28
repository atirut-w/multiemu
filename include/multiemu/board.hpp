#pragma once

#include "argparse/argparse.hpp"
#include "multiemu/cpu.hpp"
#include "multiemu/device.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace MultiEmu {

// Forward declaration
class Bus;

/**
 * Information about a bus exposed for debugging
 */
struct BusInfo {
  std::string name;        // Display name of the bus
  Bus* bus;                // Pointer to the bus
  std::function<size_t()> getProgramCounter; // Function to get program counter for highlighting (nullptr if not applicable)
};

/**
 * Base class for all emulated boards - inherits from Device
 */
class Board : public Device {
public:
  virtual ~Board() = default;

  bool display = false;
  int clock_speed = 4000000;

  // Device interface implementation
  virtual std::string getDeviceType() const override { return "board"; }
  virtual void initialize() override { /* Will be handled by setup() */ }

  /**
   * Set up the board with the given arguments
   * @param args Command line arguments
   */
  virtual void setup(const argparse::ArgumentParser &args) = 0;
  
  /**
   * Run the board for a set number of minimum cycles
   * Override Device::execute(int) to handle all child devices
   * @param cycles Minimum number of cycles to run
   * @return The actual number of cycles that were run
   */
  virtual int execute(int cycles) override = 0;
  
  /**
   * Draw graphics to the screen (no-op by default)
   */
  virtual void draw() {}
  
  /**
   * Get information about buses available for debugging
   * @return Vector of BusInfo structures
   */
  virtual std::vector<BusInfo> get_buses() const { return {}; }
  
  /**
   * Backward compatibility method - find CPU device in children
   * @return Pointer to the primary CPU
   */
  virtual CPU* getCPU();
  
  /**
   * Run method is just an alias to execute to maintain backward compatibility
   */
  int run(int cycles) { return execute(cycles); }
};

} // namespace MultiEmu
