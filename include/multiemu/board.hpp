#pragma once

#include "argparse/argparse.hpp"
#include "multiemu/cpu.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <any>

namespace MultiEmu {

/**
 * Defines the address width for buses to enable proper type casting
 */
enum class BusWidth {
  BUS_8BIT,
  BUS_16BIT,
  BUS_32BIT
};

/**
 * Information about a bus exposed for debugging
 */
struct BusInfo {
  std::string name;        // Display name of the bus
  size_t size;             // Size of addressable space
  std::any bus_ptr;        // Pointer to the bus (stored as std::any)
  BusWidth addressWidth;   // Width of address bus
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
  std::unique_ptr<CPU> cpu;

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
   * Get information about buses available for debugging
   * @return Vector of BusInfo structures
   */
  virtual std::vector<BusInfo> get_buses() const { return {}; }
};

} // namespace MultiEmu
