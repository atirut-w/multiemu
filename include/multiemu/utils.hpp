#pragma once
#include <cstdint>
#include <filesystem>
#include <vector>

struct Utils {
  static std::vector<uint8_t> load_rom(const std::filesystem::path &path);
};
