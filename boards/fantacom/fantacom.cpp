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
  auto rom = Utils::load_rom(args.get<filesystem::path>("program"));

  for (int i = 0; i < min(0x4000, static_cast<int>(rom.size())); i++) {
    board->rom[i] = rom[i];
  }

  return board;
}
REGISTER_BOARD(fantacom, create_fantacom);

FantacomBoard::FantacomBoard() {
  cpu.setupCallback(read, write, nullptr, nullptr, this);
  display = true;
  mmu.pagemap[0] = 0;

  io.add_listener(&mmu);
}

int FantacomBoard::run(int cycles) {
  return cpu.execute(cycles);
}

FantacomBoard *FantacomBoard::get_self(void *ctx) {
  return static_cast<FantacomBoard *>(ctx);
}

uint8_t FantacomBoard::read(void *ctx, uint16_t address) {
  int physical = get_self(ctx)->mmu.translate(address);

  if (physical < 0x4000) {
    return get_self(ctx)->rom[physical];
  }

  cout << "TODO: read from " << hex << physical << endl;
  
  return 0;
}

void FantacomBoard::write(void *ctx, uint16_t address, uint8_t value) {
}

uint8_t FantacomBoard::in(void *ctx, uint16_t address) {
  return get_self(ctx)->io.read(address);
}

void FantacomBoard::out(void *ctx, uint16_t address, uint8_t value) {
  get_self(ctx)->io.write(address, value);
}
