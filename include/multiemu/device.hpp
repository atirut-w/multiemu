#pragma once
#include "argparse/argparse.hpp"

namespace MultiEmu {
struct Device {
  virtual void init(const argparse::ArgumentParser &args) {}
};
} // namespace MultiEmu
