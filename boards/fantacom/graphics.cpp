#include "fantacom/graphics.hpp"
#include <cstdint>

#define TILES_ADDR 0
#define TILEMAP_ADDR 3

using namespace std;

uint8_t Graphics::read(std::size_t addr) {
  return addr < data.size() ? data[addr] : 0;
}

void Graphics::write(std::size_t addr, uint8_t data) {
  if (addr < this->data.size()) {
    this->data[addr] = data;
  }
}

void Graphics::draw() {
  auto tiles = get_address(TILES_ADDR);
  auto tilemap = get_address(TILEMAP_ADDR);
}
