#include "fantacom/fantacom.hpp"
#include "argparse/argparse.hpp"
#include "multiemu/board.hpp"
#include "multiemu/units.hpp"
#include "multiemu/utils.hpp"
#include <cstdint>
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

FantacomBoard::FantacomBoard(const ArgumentParser &args) : ram(args.get<int>("--ram") - ROM_SIZE) {
  cpu.setupCallback(read, write, in, out, this, true);
  clock_speed = 2500000; // 2.5 MHz
  display = true;

  rom.data = Utils::load_rom(args.get<filesystem::path>("program"));
  rom.data.resize(ROM_SIZE);
  rom.size = ROM_SIZE;

  virt.physical = &phys;
  gfx.ram = &phys;
  
  phys.add_subregion(&rom, 0, 1);
  phys.add_subregion(&ram, ROM_SIZE);

  io.add_subregion(&virt.config, 0);
  io.add_subregion(&gfx.config, 16);
}

int FantacomBoard::run(int cycles) { return cpu.execute(cycles); }

void FantacomBoard::draw() { gfx.draw(); }

FantacomBoard *FantacomBoard::get_self(void *ctx) { return static_cast<FantacomBoard *>(ctx); }

uint8_t FantacomBoard::read(void *ctx, uint16_t address) {
  auto *self = get_self(ctx);
  return self->virt.read(address);
}

void FantacomBoard::write(void *ctx, uint16_t address, uint8_t value) {
  auto *self = get_self(ctx);
  self->virt.write(address, value);
}

uint8_t FantacomBoard::in(void *ctx, uint16_t address) { return get_self(ctx)->io.read(address); }

void FantacomBoard::out(void *ctx, uint16_t address, uint8_t value) { get_self(ctx)->io.write(address, value); }
