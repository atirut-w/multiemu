#include "multiemu/display.hpp"
#include "raylib.h"
#include <argparse/argparse.hpp>
#include <board_registry.hpp>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <multiemu/board.hpp>
#include <ratio>
#include <thread>

using namespace std;
using namespace argparse;

unique_ptr<const ArgumentParser> parse_arguments(int argc, const char *argv[]) {
  auto parser = make_unique<ArgumentParser>("multiemu");

  auto &mutex = parser->add_mutually_exclusive_group(true);

  // Board to emulate
  mutex.add_argument("-b", "--board").help("Board to emulate");

  // List available boards
  mutex.add_argument("-l", "--list").help("List available boards").flag();

  // Program or ROM to load
  parser->add_argument("program")
      .help("Program or ROM to load")
      .action([](const string &value) { return filesystem::path(value); });

  try {
    parser->parse_args(argc, argv);
    return parser;
  } catch (const exception &error) {
    cerr << error.what() << endl;
    cerr << *parser;
    return nullptr;
  }
}

void list_boards() {
  cout << "Available boards:" << endl;
  for (const auto *board : BoardRegistry::get_board_infos()) {
    cout << "\t" << board->name << endl;
  }
}

int main(int argc, const char *argv[]) {
  auto args = parse_arguments(argc, argv);
  if (!args) {
    return 1;
  }

  if (args->get<bool>("--list")) {
    list_boards();
    return 0;
  }

  auto board_info = BoardRegistry::get_board_info(args->get<string>("board"));
  if (!board_info) {
    cerr << "Board not found: " << args->get<string>("board") << endl;
    list_boards();
    return 1;
  }

  auto board = board_info->create(*args);
  if (board->display) {
    Display::init(640, 400);
  }

  int bias = 0;
  while (true) {
    int target_cycles;
    if (board->display) {
      target_cycles = (float)board->clock_speed * GetFrameTime();
    } else {
      target_cycles = (float)board->clock_speed / 60;
      this_thread::sleep_for(chrono::duration<int64_t, ratio<1, 60>>(1));
    }

    target_cycles = target_cycles > 0 ? target_cycles : 1;
    int cycles_ran = board->run(target_cycles + bias);
    bias = target_cycles - cycles_ran;

    if (cycles_ran == 0 || (board->display && WindowShouldClose())) {
      break;
    }

    if (board->display) {
      BeginTextureMode(*Display::framebuffer);
      ClearBackground(BLACK);
      board->draw();
      EndTextureMode();
      Display::draw();
    }
  }

  return 0;
}
