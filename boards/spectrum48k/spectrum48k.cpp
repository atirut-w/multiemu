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
  
  // Initialize all keyboard keys as released (true = released)
  for (auto& row : keyboard) {
    row.fill(true);
  }
  
  // Initialize interrupt counter
  interrupt_cycles = 0;

  // Reset the CPU
  z80.reset();
  Display::init(256, 192); // Initialize display with ZX Spectrum resolution
}

void Spectrum48K::handle_keypress(int row, int col, bool released) {
  if (row >= 0 && row < 8 && col >= 0 && col < 5) {
    keyboard[row][col] = released;
  }
}

int Spectrum48K::run(int cycles) {
  // Process keyboard input
  // ZX Spectrum 48K Keyboard Matrix:
  // 0: CAPS SHIFT, Z, X, C, V
  // 1: A, S, D, F, G
  // 2: Q, W, E, R, T
  // 3: 1, 2, 3, 4, 5
  // 4: 0, 9, 8, 7, 6
  // 5: P, O, I, U, Y
  // 6: ENTER, L, K, J, H
  // 7: SPACE, SYMBOL SHIFT, M, N, B
  
  // Map PC keyboard to ZX Spectrum keyboard matrix
  // Row 0
  handle_keypress(0, 0, !IsKeyDown(KEY_LEFT_SHIFT));  // CAPS SHIFT
  handle_keypress(0, 1, !IsKeyDown(KEY_Z));          // Z
  handle_keypress(0, 2, !IsKeyDown(KEY_X));          // X
  handle_keypress(0, 3, !IsKeyDown(KEY_C));          // C
  handle_keypress(0, 4, !IsKeyDown(KEY_V));          // V
  
  // Row 1
  handle_keypress(1, 0, !IsKeyDown(KEY_A));          // A
  handle_keypress(1, 1, !IsKeyDown(KEY_S));          // S
  handle_keypress(1, 2, !IsKeyDown(KEY_D));          // D
  handle_keypress(1, 3, !IsKeyDown(KEY_F));          // F
  handle_keypress(1, 4, !IsKeyDown(KEY_G));          // G
  
  // Row 2
  handle_keypress(2, 0, !IsKeyDown(KEY_Q));          // Q
  handle_keypress(2, 1, !IsKeyDown(KEY_W));          // W
  handle_keypress(2, 2, !IsKeyDown(KEY_E));          // E
  handle_keypress(2, 3, !IsKeyDown(KEY_R));          // R
  handle_keypress(2, 4, !IsKeyDown(KEY_T));          // T
  
  // Row 3
  handle_keypress(3, 0, !IsKeyDown(KEY_ONE));        // 1
  handle_keypress(3, 1, !IsKeyDown(KEY_TWO));        // 2
  handle_keypress(3, 2, !IsKeyDown(KEY_THREE));      // 3
  handle_keypress(3, 3, !IsKeyDown(KEY_FOUR));       // 4
  handle_keypress(3, 4, !IsKeyDown(KEY_FIVE));       // 5
  
  // Row 4
  handle_keypress(4, 0, !IsKeyDown(KEY_ZERO));       // 0
  handle_keypress(4, 1, !IsKeyDown(KEY_NINE));       // 9
  handle_keypress(4, 2, !IsKeyDown(KEY_EIGHT));      // 8
  handle_keypress(4, 3, !IsKeyDown(KEY_SEVEN));      // 7
  handle_keypress(4, 4, !IsKeyDown(KEY_SIX));        // 6
  
  // Row 5
  handle_keypress(5, 0, !IsKeyDown(KEY_P));          // P
  handle_keypress(5, 1, !IsKeyDown(KEY_O));          // O
  handle_keypress(5, 2, !IsKeyDown(KEY_I));          // I
  handle_keypress(5, 3, !IsKeyDown(KEY_U));          // U
  handle_keypress(5, 4, !IsKeyDown(KEY_Y));          // Y
  
  // Row 6
  handle_keypress(6, 0, !IsKeyDown(KEY_ENTER));      // ENTER
  handle_keypress(6, 1, !IsKeyDown(KEY_L));          // L
  handle_keypress(6, 2, !IsKeyDown(KEY_K));          // K
  handle_keypress(6, 3, !IsKeyDown(KEY_J));          // J
  handle_keypress(6, 4, !IsKeyDown(KEY_H));          // H
  
  // Row 7
  handle_keypress(7, 0, !IsKeyDown(KEY_SPACE));      // SPACE
  handle_keypress(7, 1, !IsKeyDown(KEY_RIGHT_SHIFT)); // SYMBOL SHIFT
  handle_keypress(7, 2, !IsKeyDown(KEY_M));          // M
  handle_keypress(7, 3, !IsKeyDown(KEY_N));          // N
  handle_keypress(7, 4, !IsKeyDown(KEY_B));          // B
  
  // Execute CPU cycles
  int executed_cycles = 0;
  try {
    executed_cycles = z80.execute(cycles);
    
    // Update interrupt counter
    interrupt_cycles += executed_cycles;
    
    // Check if it's time for an interrupt (50Hz)
    if (interrupt_cycles >= CYCLES_PER_INTERRUPT) {
      // Generate interrupt (ZX Spectrum uses maskable interrupts with no specific vector)
      z80.requestInterrupt();
      
      // Reset counter (keeping remainder for accurate timing)
      interrupt_cycles -= CYCLES_PER_INTERRUPT;
    }
    
    return executed_cycles;
  } catch (const std::exception &e) {
    std::cerr << "CPU execution error: " << e.what() << std::endl;
    return -1;
  }
}

uint8_t Spectrum48K::read_memory(uint16_t address) { return memory_bus.read16(address); }

void Spectrum48K::write_memory(uint16_t address, uint8_t value) { memory_bus.write16(address, value); }

uint8_t Spectrum48K::read_port(uint16_t port) {
  // Keyboard ports - The ZX Spectrum uses these ports for keyboard rows:
  // 0xFEFE (11111110 11111110) - Row 0: CAPS SHIFT, Z, X, C, V
  // 0xFDFE (11111101 11111110) - Row 1: A, S, D, F, G
  // 0xFBFE (11111011 11111110) - Row 2: Q, W, E, R, T
  // 0xF7FE (11110111 11111110) - Row 3: 1, 2, 3, 4, 5
  // 0xEFFE (11101111 11111110) - Row 4: 0, 9, 8, 7, 6
  // 0xDFFE (11011111 11111110) - Row 5: P, O, I, U, Y
  // 0xBFFE (10111111 11111110) - Row 6: ENTER, L, K, J, H
  // 0x7FFE (01111111 11111110) - Row 7: SPACE, SYMBOL SHIFT, M, N, B
  
  // Check if the low byte is 0xFE (all keyboard ports end with FE)
  if ((port & 0x00FF) == 0xFE) {
    uint8_t result = 0xFF; // Initialize all bits to 1
    
    // In the ZX Spectrum, bits 5-7 of the result should be 1
    // However, we're only using bits 0-4 for the keys, so no need to explicitly set them
    
    // The high byte of the port address selects the keyboard half-row
    // A half-row is selected when its bit in the high byte is LOW (0)
    uint8_t high_byte = (port >> 8) & 0xFF;
    
    // Check each row
    for (int row = 0; row < 8; row++) {
      // If this row is selected (bit is 0 in the high byte)
      if (!(high_byte & (1 << row))) {
        // Check each key in the row
        for (int col = 0; col < 5; col++) {
          // If key is pressed (false), clear the corresponding bit
          if (!keyboard[row][col]) {
            result &= ~(1 << col);
          }
        }
        // We found the selected row, no need to check others
        break;
      }
    }
    
    return result;
  }
  
  // Default for unhandled ports
  return 0xFF;
}

void Spectrum48K::write_port(uint16_t port, uint8_t value) {
  // ULA port (border color, etc.)
  if ((port & 0x00FF) == 0xFE) {
    // Border color is in bits 0-2
    // TODO: Implement border color setting
  }
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
