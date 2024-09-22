#pragma once
#include <memory>
#include <string>

struct Board {
  virtual ~Board() = default;
};

struct BoardInfo {
  std::string name;
  std::unique_ptr<Board> (*create)();
};

#define REGISTER_BOARD(name, ctor)                                             \
  __attribute__((section("boards"))) static BoardInfo name##_board_info = {    \
      #name, ctor};
