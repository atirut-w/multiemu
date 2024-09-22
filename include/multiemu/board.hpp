#pragma once
#include <memory>
#include <string>

struct Board {
  virtual ~Board() = default;

  // Run the board for one step (instruction or cycle), and return the number of instructions or cycles executed.
  virtual int step() = 0;
};

struct BoardInfo {
  std::string name;
  std::unique_ptr<Board> (*create)();
};

#define REGISTER_BOARD(name, ctor)                                             \
  __attribute__((section("boards"))) static BoardInfo name##_board_info = {    \
      #name, ctor};
