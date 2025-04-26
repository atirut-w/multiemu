#pragma once

#include "argparse/argparse.hpp"
#include "multiemu/cpu.hpp"
#include "multiemu/address_space.hpp"
#include <memory>
#include <string>
#include <vector>

namespace MultiEmu {

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

private:
  std::vector<AddressSpace *> address_spaces;

public:
  /**
   * Get all address spaces exposed by this board
   */
  const std::vector<AddressSpace *>& get_address_spaces() const {
    return address_spaces;
  }

  /**
   * Add a new address space to the board
   */
  void add_address_space(AddressSpace *space) {
    address_spaces.push_back(space);
  }
};

} // namespace MultiEmu
