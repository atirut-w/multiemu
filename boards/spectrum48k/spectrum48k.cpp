#include "spectrum48k/spectrum48k.hpp"
#include "multiemu/board_registry.hpp"
#include "multiemu/display.hpp"
#include "multiemu/utils.hpp"
#include "raylib.h"
#include <argparse/argparse.hpp>
#include <array>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <stdexcept>

static std::array<Color, 16> palette = {
    Color{0, 0, 0, 255},      // Black
    Color{1, 0, 206, 255},    // Blue
    Color{207, 1, 0, 255},    // Red
    Color{207, 1, 206, 255},  // Magenta
    Color{0, 207, 21, 255},   // Green
    Color{1, 207, 207, 255},  // Cyan
    Color{207, 207, 21, 255}, // Yellow
    Color{207, 207, 207, 255},// White
    Color{0, 0, 0, 255},      // Bright Black
    Color{2, 0, 253, 255},    // Bright Blue
    Color{255, 2, 1, 255},    // Bright Red
    Color{255, 2, 253, 255},  // Bright Magenta
    Color{0, 255, 28, 255},   // Bright Green
    Color{2, 255, 255, 255},  // Bright Cyan
    Color{255, 241, 29, 255}, // Bright Yellow
    Color{255, 255, 255, 255} // Bright White
};

namespace MultiEmu {

// Register the board with the registry
static BoardRegistry::Register<Spectrum48K> registration("spectrum48k", "ZX Spectrum 48K");

Spectrum48K::Spectrum48K() {
  // Default constructor - initialization will happen in setup()
}

void Spectrum48K::setup(const argparse::ArgumentParser &args) {
  // Set up CPU memory and I/O callbacks
  z80.read = [this](uint16_t address) { return read_memory(address); };
  z80.write = [this](uint16_t address, uint8_t value) { write_memory(address, value); };
  z80.in = [this](uint16_t port) { return read_port(port); };
  z80.out = [this](uint16_t port, uint8_t value) { write_port(port, value); };

  // Set clock speed (3.5MHz for ZX Spectrum)
  clock_speed = 3500000;
  display = true;

  // Load ROM
  try {
    std::filesystem::path rom_path = args.get<std::filesystem::path>("program");
    rom = Utils::load_rom(rom_path, 16 * 1024);
  } catch (const std::exception &e) {
    std::cerr << "Failed to load ROM: " << e.what() << std::endl;
    // Default to empty ROM - will return 0xFF for all reads
    rom.resize(16 * 1024, 0xFF);
  }

  // Set up memory bus (16K ROM + 48K RAM)
  memory_bus.mapRegion16(
      0x0000, 0x3fff, // 16K ROM (0x0000-0x3FFF)
      [this](uint16_t addr) -> uint8_t { return rom[addr]; },
      [this](uint16_t addr, uint8_t val) { /* ROM is read-only */ }, 0);

  memory_bus.mapRegion16(
      0x4000, 0xffff, // 48K RAM (0x4000-0xFFFF)
      [this](uint16_t addr) -> uint8_t { return ram[addr]; }, 
      [this](uint16_t addr, uint8_t val) { ram[addr] = val; },
      0);

  // Initialize RAM to 0
  std::fill(ram.begin(), ram.end(), 0);

  // Reset the CPU
  z80.reset();
  Display::init(256, 192); // Initialize display with ZX Spectrum resolution
}

int Spectrum48K::run(int cycles) {
  try {
    return z80.execute(cycles);
  } catch (const std::exception &e) {
    std::cerr << "CPU execution error: " << e.what() << std::endl;
    return -1;
  }
}

uint8_t Spectrum48K::read_memory(uint16_t address) { return memory_bus.read16(address); }

void Spectrum48K::write_memory(uint16_t address, uint8_t value) { memory_bus.write16(address, value); }

uint8_t Spectrum48K::read_port(uint16_t port) {
  // Stub implementation
  // TODO: Implement ULA ports, keyboard, etc.
  return 0xFF;
}

void Spectrum48K::write_port(uint16_t port, uint8_t value) {
  // Stub implementation
  // TODO: Implement ULA ports (border color, etc.)
}

void Spectrum48K::draw() {
  auto frame = GenImageColor(256, 192, BLACK);

  for (int line = 0; line < 192; ++line) {
    for (int byte = 0; byte < 32; ++byte) {
      uint16_t byte_addr = 0x4000 | ((line & 0xC0) << 5) | ((line & 0x07) << 8) | ((line & 0x38) << 2) | byte;
      uint16_t attr_addr = 0x5800 + (line / 8) * 32 + byte;

      uint8_t pixelByte = memory_bus.read16(byte_addr);
      uint8_t attrByte = memory_bus.read16(attr_addr);
      uint8_t ink = (attrByte & 0x07);
      uint8_t paper = (attrByte >> 3) & 0x07;
      uint8_t flash = (attrByte >> 6) & 0x01;
      uint8_t bright = (attrByte >> 7) & 0x01;
      Color inkColor = palette[ink + (bright ? 8 : 0)];
      Color paperColor = palette[paper + (bright ? 8 : 0)];
      for (int bit = 0; bit < 8; ++bit) {
        uint8_t color = (pixelByte >> (7 - bit)) & 1;
        Color pixelColor = (color == 1) ? inkColor : paperColor;

        // Draw the pixel
        ImageDrawPixel(&frame, byte * 8 + bit, line, pixelColor);
      }
    }
  }

  auto texture = LoadTextureFromImage(frame);
  UnloadImage(frame);
  Display::begin();
  DrawTexture(texture, 0, 0, WHITE);
  Display::end();
  UnloadTexture(texture);
}

std::vector<BusInfo> Spectrum48K::get_buses() const {
  std::vector<BusInfo> buses;

  // Add memory bus
  BusInfo memBus;
  memBus.name = "Memory";
  memBus.bus = const_cast<Bus *>(&memory_bus);
  memBus.getProgramCounter = [this]() { return static_cast<size_t>(z80.getProgramCounter()); };
  buses.push_back(memBus);

  return buses;
}

} // namespace MultiEmu
