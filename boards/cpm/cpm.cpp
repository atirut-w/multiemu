#include "cpm.hpp"
#include "argparse/argparse.hpp"
#include "multiemu/board_registry.hpp"
#include "multiemu/units.hpp"
#include "multiemu/utils.hpp"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <z80.hpp>

using namespace std;
using namespace argparse;
using namespace MultiEmu;

static BoardRegistry::Register<CPMBoard> registration("cpm", "CP/M userland emulation with Z80 processor");

CPMBoard *get_self(void *self) { return static_cast<CPMBoard *>(self); }

void call_handler(void *ctx) {
  auto board = get_self(ctx);
  auto &cpu = board->cpu;

  if (cpu.reg.PC == 5)
  {
    uint8_t syscall = cpu.reg.pair.C;
    board->memory[5] = 0xc9;

    switch (syscall)
    {
    default:
      cout << "Unknown syscall: " << static_cast<int>(syscall) << "\n";
      // "Dump it"
      cout << "Registers:\n";
      printf("AF: %04x\n", (cpu.reg.pair.A << 8) | cpu.reg.pair.F);
      printf("BC: %04x\n", (cpu.reg.pair.B << 8) | cpu.reg.pair.C);
      printf("DE: %04x\n", (cpu.reg.pair.D << 8) | cpu.reg.pair.E);
      printf("HL: %04x\n", (cpu.reg.pair.H << 8) | cpu.reg.pair.L);
      break;
    case 0:
      board->running = false;
      break;
    case 2:
      cout << cpu.reg.pair.E;
      break;
    }
  }
}

uint8_t read(void *ctx, uint16_t address) {
  return static_cast<CPMBoard *>(ctx)->memory[address];
}

void write(void *ctx, uint16_t address, uint8_t value) {
  static_cast<CPMBoard *>(ctx)->memory[address] = value;
}

void CPMBoard::setup(const ArgumentParser &args) {
  ifstream rom(args.get<filesystem::path>("program"), ios::binary);
  auto romv = Utils::load_rom(args.get<filesystem::path>("program"), 64 * KIB);
  
  for (int i = 0; i < min(0x10000, static_cast<int>(romv.size())); i++)
  {
    memory[i] = romv[i];
  }

  cpu.reg.PC = 0x100;
  cpu.reg.SP = 0;

  cpu.setupCallback(read, write, nullptr, nullptr, this);
  cpu.addCallHandler(call_handler);
}

int CPMBoard::run(int cycles) {
  if (running) {
    return cpu.execute(cycles);
  } else {
    return 0;
  }
}
