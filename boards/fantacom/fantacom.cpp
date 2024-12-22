#include "fantacom/fantacom.hpp"
#include "argparse/argparse.hpp"
#include "multiemu/board.hpp"
#include "multiemu/units.hpp"
#include "multiemu/utils.hpp"
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <memory>

#define ROM_SIZE 0x2000

using namespace std;
using namespace argparse;
using namespace MultiEmu;

unique_ptr<Board> create_fantacom(const ArgumentParser &args) {
  auto board = make_unique<FantacomBoard>(args);
  return board;
}
REGISTER_BOARD(fantacom, create_fantacom);

FantacomBoard::FantacomBoard(const ArgumentParser &args) {
  cpu.setupCallback(read, write, in, out, this, true);
  clock_speed = 2500000; // 2.5 MHz
  display = true;

  phys = make_unique<MemoryRegion>(MIB);

  rom = make_unique<MemoryRegionROM>(ROM_SIZE);
  rom->data = Utils::load_rom(args.get<filesystem::path>("program"));
  rom->data.resize(ROM_SIZE);
  phys->add_subregion(rom.get(), 0);

  int ram_size = args.get<int>("--ram");
  ram = make_unique<MemoryRegionRAM>(std::min(ram_size, MIB - ROM_SIZE));
  phys->add_subregion(ram.get(), ROM_SIZE);

  io = make_unique<MemoryRegion>(64 * KIB);
  io->add_subregion(&mmu_config, 0);
  io->add_subregion(&gfx.config, 16);

  gfx.ram = phys.get();
}

int FantacomBoard::run(int cycles) { return cpu.execute(cycles); }

void FantacomBoard::draw() { gfx.draw(); }

FantacomBoard *FantacomBoard::get_self(void *ctx) { return static_cast<FantacomBoard *>(ctx); }

uint8_t FantacomBoard::read(void *ctx, uint16_t address) {
  auto *self = get_self(ctx);
  auto pagemap = self->mmu_config.data;

  int v_page = address >> 12;
  int p_page = pagemap[v_page];
  int p_addr = (p_page << 12) | (address & 0xfff);
  return self->phys->read(p_addr);
}

void FantacomBoard::write(void *ctx, uint16_t address, uint8_t value) {
  auto *self = get_self(ctx);
  auto pagemap = self->mmu_config.data;

  int v_page = address >> 12;
  int p_page = pagemap[v_page];
  int p_addr = (p_page << 12) | (address & 0xfff);
  self->phys->write(p_addr, value);
}

uint8_t FantacomBoard::in(void *ctx, uint16_t address) { return get_self(ctx)->io->read(address); }

void FantacomBoard::out(void *ctx, uint16_t address, uint8_t value) { get_self(ctx)->io->write(address, value); }
