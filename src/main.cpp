#include "imgui.h"
#include "multiemu/display.hpp"
#include "raylib.h"
#include "rlImGui.h"
#include <algorithm>
#include <argparse/argparse.hpp>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <multiemu/board.hpp>
#include <multiemu/board_registry.hpp>
#include <ratio>
#include <thread>

using namespace std;
using namespace argparse;
using namespace MultiEmu;

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
  
  // Amount of RAM in KB
  parser->add_argument("-r", "--ram")
      .help("Amount of RAM in KB")
      .default_value(64 * 1024)
      .action([](const string &value) { return stoi(value) * 1024; });

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
  for (const auto &[name, info] : BoardRegistry::get_board_infos()) {
    cout << "\t" << name << "\t" << info.description << endl;
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

  auto board = BoardRegistry::create_board(args->get<string>("board"));
  if (!board) {
    cerr << "Board not found: " << args->get<string>("board") << endl;
    list_boards();
    return 1;
  }

  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(640, 400, "MultiEmu");
  rlImGuiSetup(true); // Dark theme my beloved
  bool run = true;
  bool showAbout = false;

  while (!WindowShouldClose() && run) {
    BeginDrawing();
    ClearBackground(GRAY);

    // TODO: Draw the UI here
    rlImGuiBegin();

    if (ImGui::BeginMainMenuBar()) {
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Quit")) {
          run = false;
        }
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Help")) {
        if (ImGui::MenuItem("About")) {
          showAbout = true;
        }
        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    }

    if (showAbout) {
      ImGui::Begin("About", &showAbout, ImGuiWindowFlags_AlwaysAutoResize);
      ImGui::Text("MultiEmu");
      ImGui::Text("A multi-system emulator framework");
      ImGui::End();
    }

    rlImGuiEnd();

    EndDrawing();
  }
  
  // board->setup(*args);
  // if (board->display) {
  //   Display::init(640, 400);
  // }

  // int bias = 0;
  // while (true) {
  //   int target_cycles;
  //   if (board->display) {
  //     float frame_time = GetFrameTime();
  //     if (frame_time > 5.0) {
  //       cout << "Warning: Unusual frame time (" << frame_time << "). Did WM lose focus?" << endl;
  //       frame_time = 1.0 / 60;
  //     }
  //     target_cycles = (float)board->clock_speed * frame_time;
  //   } else {
  //     target_cycles = (float)board->clock_speed / 60;
  //     this_thread::sleep_for(chrono::duration<int64_t, ratio<1, 60>>(1));
  //   }

  //   int cycles_ran = board->run(max(1, target_cycles + bias));
  //   bias = target_cycles - cycles_ran;

  //   if (cycles_ran == 0 || (board->display && WindowShouldClose())) {
  //     break;
  //   }

  //   if (board->display) {
  //     board->draw();
  //     Display::draw();
  //   }
  // }

  return 0;
}
