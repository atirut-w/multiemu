#pragma once

#include "argparse/argparse.hpp"
#include <string>
namespace MultiEmu {

class Board {
public:
  virtual ~Board() = default;

  std::string name;

  bool display = false;
  int clock_speed = 4000000;

  // Set up the board with the given arguments
  virtual void setup(const argparse::ArgumentParser &args) = 0;
  // Run the board for a set number of minimum cycles and return the actual
  // cycles ran
  virtual int run(int cycles) = 0;
  // Draw graphics to the screen
  virtual void draw() {}
};

} // namespace MultiEmu
