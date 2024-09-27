#pragma once
#include <cstdint>
#include <filesystem>
#include <vector>

namespace MultiEmu::Utils {
  std::vector<uint8_t> load_rom(const std::filesystem::path &path);
}
