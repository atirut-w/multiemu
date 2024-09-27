#include "multiemu/utils.hpp"
#include <filesystem>
#include <fstream>

using namespace std;
using namespace MultiEmu;

vector<uint8_t> Utils::load_rom(const filesystem::path &path) {
  ifstream file(path, ios::binary);
  if (!file) {
    throw runtime_error("Cannot open ROM file");
  }

  vector<uint8_t> rom;
  file.seekg(0, ios::end);
  size_t size = file.tellg();
  file.seekg(0, ios::beg);
  rom.resize(size);
  file.read(reinterpret_cast<char *>(rom.data()), size);
  
  return rom;
}
