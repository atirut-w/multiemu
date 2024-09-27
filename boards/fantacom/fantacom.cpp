#include "fantacom/fantacom.hpp"
#include "argparse/argparse.hpp"
#include "multiemu/board.hpp"
#include "multiemu/utils.hpp"
#include <cstdint>
#include <memory>

using namespace std;
using namespace argparse;

unique_ptr<Board> create_fantacom(const ArgumentParser &args) {
  auto board = make_unique<FantacomBoard>();
  board->rom.data = Utils::load_rom(args.get<filesystem::path>("program"));
  board->rom.data.resize(0x4000);
  board->ram.data.resize(args.get<int>("--ram"));

  return board;
}
REGISTER_BOARD(fantacom, create_fantacom);

FantacomBoard::FantacomBoard() {
  cpu.setupCallback(read, write, in, out, this);
  display = true;
  ram.offset = 0x4000;
  mmu.pagemap.fill(0);

  rambus.add_listener(&rom);
  rambus.add_listener(&ram);
  io.add_listener(&mmu);
  io.add_listener(&gfx);
}

int FantacomBoard::run(int cycles) {
  return cpu.execute(cycles);
}

void FantacomBoard::draw() {
  gfx.draw();
}

FantacomBoard *FantacomBoard::get_self(void *ctx) {
  return static_cast<FantacomBoard *>(ctx);
}

uint8_t FantacomBoard::read(void *ctx, uint16_t address) {
  auto *self = get_self(ctx);
  return self->rambus.read(self->mmu.translate(address));
}

void FantacomBoard::write(void *ctx, uint16_t address, uint8_t value) {
  auto *self = get_self(ctx);
  self->rambus.write(self->mmu.translate(address), value);
}

uint8_t FantacomBoard::in(void *ctx, uint16_t address) {
  return get_self(ctx)->io.read(address);
}

void FantacomBoard::out(void *ctx, uint16_t address, uint8_t value) {
  get_self(ctx)->io.write(address, value);
}
