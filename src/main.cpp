#include "raylib.h"
#include <argparse/argparse.hpp>
#include <board_registry.hpp>
#include <filesystem>
#include <iostream>
#include <memory>
#include <multiemu/board.hpp>

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

  auto &spec = board_info->spec;
  if (spec.display) {
    // Default resolution: 80x25 (8x16)
    int width = 640;
    int height = 400;
    InitWindow(width, height, "MultiEmu");
    auto board = board_info->create(*args); // Give boards a chance to set up the display

    const char *no_disp_msg = "NO VIDEO OUTPUT";
    int text_width = MeasureText(no_disp_msg, 20);

    // Turns out, Raylib uses double buffering
    for (int i = 0; i < 25; i++) {
      BeginDrawing();
      ClearBackground(BLACK);
      DrawText(no_disp_msg, width / 2 - text_width / 2, height / 2 - 10, 20,
              WHITE);
      EndDrawing();
    }

    while (!WindowShouldClose()) {
      BeginDrawing();

      if (!board->run(1)) {
        cout << "Board stopped" << endl;
        CloseWindow();
        break;
      }

      EndDrawing();
    }
  } else {
    auto board = board_info->create(*args);
    while (true) {
      if (!board->run(1)) {
        cout << "Board stopped" << endl;
        break;
      }
    }
  }

  return 0;
}
