#pragma once

#include "argparse/argparse.hpp"
#include "multiemu/cpu.hpp"
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
 * Base class for all emulated boards
 */
class Board {
public:
  virtual ~Board() = default;

  bool display = false;
  int clock_speed = 4000000;

  /**
   * Set up the board with the given arguments
   * @param args Command line arguments
   */
  virtual void setup(const argparse::ArgumentParser &args) = 0;
  
  /**
   * Run the board for a set number of minimum cycles
   * @param cycles Minimum number of cycles to run
   * @return The actual number of cycles that were run
   */
  virtual int run(int cycles) = 0;
  /**
   * Draw graphics to the screen (no-op by default)
   */
  virtual void draw() {}
  
  /**
   * Called at the end of each frame (vertical blanking interval)
   * Boards can implement this to handle frame-synchronized events like interrupts
   */
  virtual void vblank() {}
  
  
  /**
   * Get information about buses available for debugging
   * @return Vector of BusInfo structures
   */
  virtual std::vector<BusInfo> get_buses() const { return {}; }
  
  /**
   * Get the CPU associated with this board
   * @return Pointer to the CPU
   */
  virtual CPU* getCPU() = 0;
};

} // namespace MultiEmu
