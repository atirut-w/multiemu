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

  rom = Utils::load_rom(args.get<filesystem::path>("program"), ROM_SIZE);
  ram.resize(args.get<int>("--ram"));

  // Set up physical memory layout
  phys.mapRegion(0, 256 * KIB, 
    [this](uint32_t address) -> uint8_t { return rom[address]; }, 
    [this](uint32_t address, uint8_t value) {}, 0);
  
  phys.mapRegion(256 * KIB, 256 * KIB, 
    [this](uint32_t address) -> uint8_t { return gfx.vram[address - 256 * KIB]; }, 
    [this](uint32_t address, uint8_t value) { gfx.vram[address - 256 * KIB] = value; }, 0);
  
  phys.mapRegion(512 * KIB, 512 * KIB, 
    [this](uint32_t address) -> uint8_t { 
      uint32_t offset = address - 512 * KIB;
      if (offset < ram.size()) return ram[offset]; else return 0; 
    }, 
    [this](uint32_t address, uint8_t value) { 
      uint32_t offset = address - 512 * KIB;
      if (offset < ram.size()) ram[offset] = value; 
    }, 0);

  // Virtual memory just goes through MMU
  virt.mapRegion16(0, 64 * KIB, 
    [this](uint16_t address) -> uint8_t { return read(address); }, 
    [this](uint16_t address, uint8_t value) { write(address, value); }, 0);

  // Set up I/O ports
  io.mapRegion8(0, 16, 
    [this](uint8_t address) -> uint8_t { return mmu_config[address]; }, 
    [this](uint8_t address, uint8_t value) { mmu_config[address] = value; }, 0);
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

std::vector<MultiEmu::BusInfo> FantacomBoard::get_buses() const {
  std::vector<MultiEmu::BusInfo> buses;
  
  // Add virtual memory bus (CPU view)
  MultiEmu::BusInfo virtBus;
  virtBus.name = "Memory (Virtual/CPU)";
  virtBus.bus = const_cast<MultiEmu::Bus*>(&virt);
  virtBus.getProgramCounter = [this]() { 
    return static_cast<size_t>(cpu->getProgramCounter()); 
  };
  buses.push_back(virtBus);
  
  // Add physical memory bus (after MMU translation)
  MultiEmu::BusInfo physBus;
  physBus.name = "Memory (Physical)";
  physBus.bus = const_cast<MultiEmu::Bus*>(&phys);
  physBus.getProgramCounter = [this]() {
    auto pc = cpu->getProgramCounter();
    auto& pagemap = mmu_config;
    int v_page = pc >> 12;
    int p_page = pagemap[v_page];
    return static_cast<size_t>((p_page << 12) | (pc & 0xfff));
  };
  buses.push_back(physBus);
  
  // Add I/O bus
  MultiEmu::BusInfo ioBus;
  ioBus.name = "I/O Ports";
  ioBus.bus = const_cast<MultiEmu::Bus*>(&io);
  // No program counter for I/O bus
  buses.push_back(ioBus);
  
  return buses;
}

uint8_t FantacomBoard::read(uint16_t address) {
  auto &pagemap = mmu_config;

  int v_page = address >> 12;
  int p_page = pagemap[v_page];
  int p_addr = (p_page << 12) | (address & 0xfff);
  return phys.read(p_addr);
}

void FantacomBoard::write(uint16_t address, uint8_t value) {
  auto &pagemap = mmu_config;

  int v_page = address >> 12;
  int p_page = pagemap[v_page];
  int p_addr = (p_page << 12) | (address & 0xfff);
  phys.write(p_addr, value);
}

uint8_t FantacomBoard::in(uint16_t address) { 
  return io.read8(address & 0xFF); 
}

void FantacomBoard::out(uint16_t address, uint8_t value) { 
  io.write8(address & 0xFF, value); 
}
