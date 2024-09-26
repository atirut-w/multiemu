#pragma once
#include "argparse/argparse.hpp"
#include <memory>
#include <string>

struct Board {
  virtual ~Board() = default;

  bool display = false;
  int clock_speed = 4000000;

  // Run the board for a set number of minimum cycles and return the actual
  // cycles
  virtual int run(int cycles) = 0;
  // Draw graphics to the screen
  virtual void draw() {}
};

struct BoardSpec {
  bool display = false;
};

struct BoardInfo {
  std::string name;
  std::unique_ptr<Board> (*create)(const argparse::ArgumentParser &args);
};

#define REGISTER_BOARD(name, ctor)                                             \
  __attribute__((section("boards"))) static BoardInfo name##_board_info = {    \
      #name, ctor};
