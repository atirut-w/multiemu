#pragma once
#include <cpu.hpp>
#include <memory>
#include <string>

struct Board {
  virtual ~Board() = default;

  std::unique_ptr<CPU> cpu;
};

// Dev note: Please do not put anything that modifies itself in board meta, as
// it will be placed in a read-only section
struct BoardMeta {
  const char *name;
  std::unique_ptr<Board> (*const ctor_wrapper)();
};

#define REGISTER_BOARD(name, ctor)                                             \
  __attribute__((section("boards")))                                           \
  const BoardMeta name##_meta = {#name, ctor};
