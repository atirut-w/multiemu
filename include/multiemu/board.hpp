#pragma once
#include "argparse/argparse.hpp"
#include <memory>
#include <string>

struct Board {
  virtual ~Board() = default;

  // Run the board for a set number of minimum cycles and return the actual
  // cycles
  virtual int run(int cycles) = 0;
};

struct BoardSpec {
  bool display = false;
};

struct BoardInfo {
  std::string name;
  BoardSpec spec;
  std::unique_ptr<Board> (*create)(const argparse::ArgumentParser &args);
};

#define REGISTER_BOARD(name, ctor, spec)                                       \
  __attribute__((section("boards"))) static BoardInfo name##_board_info = {    \
      #name, spec, ctor};
