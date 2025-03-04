#pragma once

#include "argparse/argparse.hpp"
#include <string>
namespace MultiEmu {

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
};

} // namespace MultiEmu
