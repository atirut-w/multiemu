#include "fantacom/fantacom.hpp"
#include "argparse/argparse.hpp"
#include "multiemu/board.hpp"
#include "multiemu/board_registry.hpp"
#include "multiemu/units.hpp"
#include "multiemu/utils.hpp"
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <stdexcept>

#define ROM_SIZE (256 * KIB)

using namespace std;
using namespace argparse;
using namespace MultiEmu;

static BoardRegistry::Register<FantacomBoard> registration("fantacom", "Z80-based fantasy computer");

void FantacomBoard::setup(const ArgumentParser &args) {
  cpu = std::make_unique<MultiEmu::Z80>();

  // Set up memory access callbacks
  cpu->read = [this](uint16_t address) { return read(address); };
  cpu->write = [this](uint16_t address, uint8_t value) { write(address, value); };
  cpu->in = [this](uint16_t address) { return in(address); };
  cpu->out = [this](uint16_t address, uint8_t value) { out(address, value); };
  clock_speed = 2500000; // 2.5 MHz
  display = true;

  // Create ROM region
  rom_region = make_unique<MemoryRegionROM>(ROM_SIZE);
  rom_region->get_data() = Utils::load_rom(args.get<filesystem::path>("program"), ROM_SIZE);
  phys.add_region(rom_region.get(), 0);

  // Create RAM region
  int ram_size = args.get<int>("--ram");
  ram_region = make_unique<MemoryRegionRAM>(std::min(ram_size, 512 * KIB));
  phys.add_region(ram_region.get(), 512 * KIB);

  // Add 256KB VRAM at 256KB into physical address space
  phys.add_region(&gfx.vram, 256 * KIB);

  // Add I/O regions to I/O address space
  io.add_region(&mmu_config, 0x00);
  io.add_region(&gfx.config, 0xf0);

  // No need to set gfx.ram since we're using dedicated VRAM now

  // Create an MMIO region for virtual memory translation (CPU-visible memory)
  virt_mmio = make_unique<MemoryRegionMMIO>(
    64 * KIB,
    [this](size_t addr) { return read(addr); },
    [this](size_t addr, uint8_t val) { write(addr, val); }
  );
  virt.add_region(virt_mmio.get(), 0);

  // Set up program counter display for virtual memory
  virt.getProgramCounter = [this]() { return static_cast<size_t>(cpu->getProgramCounter()); };

  // Set up program counter display for physical memory (after MMU translation)
  phys.getProgramCounter = [this]() {
    auto pc = cpu->getProgramCounter();
    auto& pagemap = mmu_config.get_data();

    int v_page = pc >> 12;
    int p_page = pagemap[v_page];
    int p_addr = (p_page << 12) | (pc & 0xfff);
    return p_addr;
  };

  // Add the address spaces to the board
  add_address_space(&virt);
  add_address_space(&phys);
  add_address_space(&io);
}

int FantacomBoard::run(int cycles) {
  try {
    return cpu->execute(cycles);
  } catch (const runtime_error &e) {
    cerr << "Error in CPU: " << e.what() << endl;
    return -1;
  }
}

void FantacomBoard::draw() { gfx.draw(); }

uint8_t FantacomBoard::read(uint16_t address) {
  auto& pagemap = mmu_config.get_data();

  int v_page = address >> 12;
  int p_page = pagemap[v_page];
  int p_addr = (p_page << 12) | (address & 0xfff);
  return phys.read(p_addr);
}

void FantacomBoard::write(uint16_t address, uint8_t value) {
  auto& pagemap = mmu_config.get_data();

  int v_page = address >> 12;
  int p_page = pagemap[v_page];
  int p_addr = (p_page << 12) | (address & 0xfff);
  phys.write(p_addr, value);
}

uint8_t FantacomBoard::in(uint16_t address) { return io.read(address); }

void FantacomBoard::out(uint16_t address, uint8_t value) { io.write(address, value); }
