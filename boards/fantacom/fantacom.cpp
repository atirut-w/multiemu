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

  phys = make_unique<MemoryRegion>(MIB);

  rom = make_unique<MemoryRegionROM>(ROM_SIZE);
  rom->data = Utils::load_rom(args.get<filesystem::path>("program"), ROM_SIZE);
  phys->add_subregion(rom.get(), 0);

  int ram_size = args.get<int>("--ram");
  ram = make_unique<MemoryRegionRAM>(std::min(ram_size, 512 * KIB));
  phys->add_subregion(ram.get(), 512 * KIB);

  // Add 256KB VRAM at 256KB into physical address space
  phys->add_subregion(&gfx.vram, 256 * KIB);

  io = make_unique<MemoryRegion>(256);
  io->add_subregion(&mmu_config, 0);
  io->add_subregion(&gfx.config, 16);

  gfx.ram = phys.get();

  // Create memory address space (virtual memory after MMU translation)
  auto memorySpace = std::make_unique<AddressSpace>(
      "Memory (Virtual)", AddressSpaceType::MEMORY, 64 * KIB, [this](size_t addr) { return read(addr); },
      [this](size_t addr, uint8_t val) { write(addr, val); },
      [this]() { return static_cast<size_t>(cpu->getProgramCounter()); });
  add_address_space(std::move(memorySpace));

  // Create physical memory address space (direct access, no MMU)
  auto physicalSpace = std::make_unique<AddressSpace>(
      "Memory (Physical)", AddressSpaceType::MEMORY, MIB, [this](size_t addr) { return this->phys->read(addr); },
      [this](size_t addr, uint8_t val) { this->phys->write(addr, val); },
      [this]() {
        auto pc = cpu->getProgramCounter();
        auto pagemap = mmu_config.data;

        int v_page = pc >> 12;
        int p_page = pagemap[v_page];
        int p_addr = (p_page << 12) | (pc & 0xfff);
        return p_addr;
      });
  add_address_space(std::move(physicalSpace));

  // Create I/O space
  auto ioSpace = std::make_unique<AddressSpace>(
      "I/O", AddressSpaceType::IO, 256, [this](size_t addr) { return in(addr); },
      [this](size_t addr, uint8_t val) { out(addr, val); });
  add_address_space(std::move(ioSpace));

  // Create VRAM address space (for direct video memory access)
  auto vramSpace = std::make_unique<AddressSpace>(
      "VRAM", AddressSpaceType::CUSTOM, 256 * KIB, [this](size_t addr) { return gfx.vram.read(addr); },
      [this](size_t addr, uint8_t val) { gfx.vram.write(addr, val); });
  add_address_space(std::move(vramSpace));
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
  auto pagemap = mmu_config.data;

  int v_page = address >> 12;
  int p_page = pagemap[v_page];
  int p_addr = (p_page << 12) | (address & 0xfff);
  return phys->read(p_addr);
}

void FantacomBoard::write(uint16_t address, uint8_t value) {
  auto pagemap = mmu_config.data;

  int v_page = address >> 12;
  int p_page = pagemap[v_page];
  int p_addr = (p_page << 12) | (address & 0xfff);
  phys->write(p_addr, value);
}

uint8_t FantacomBoard::in(uint16_t address) { return io->read(address); }

void FantacomBoard::out(uint16_t address, uint8_t value) { io->write(address, value); }
