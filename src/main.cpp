#include "imgui.h"
#include "multiemu/display.hpp"
#include "raylib.h"
#include "rlImGui.h"
#include "ui/debugger_window.hpp"
#include "ui/main_menu_bar.hpp"
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
  parser->add_argument("program").help("Program or ROM to load").action([](const string &value) {
    return filesystem::path(value);
  });

  // Amount of RAM in KB
  parser->add_argument("-r", "--ram")
      .help("Amount of RAM in KB")
      .default_value(64 * 1024)
      .action([](const string &value) { return stoi(value) * 1024; });
      
  // Break on start for debugging
  parser->add_argument("--break").help("Break on start for debugging").flag();

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

  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(1280, 720, "MultiEmu - Initializing...");
  SetTargetFPS(60);
  rlImGuiSetup(true); // Dark theme my beloved
  bool run = true;
  bool showAbout = false;
  int bias = 0;

  auto board = BoardRegistry::create_board(args->get<string>("board"));
  if (!board) {
    cerr << "Board not found: " << args->get<string>("board") << endl;
    list_boards();
    return 1;
  }

  // Create debugger window
  DebuggerWindow debugger(board.get());

  // Initialize menu bar
  MainMenuBar mainMenuBar;

  // File menu
  Menu fileMenu;
  fileMenu.name = "File";
  fileMenu.menuItems.push_back({"Quit", [&run]() { run = false; }});
  mainMenuBar.menus.push_back(fileMenu);

  // View menu
  Menu viewMenu;
  viewMenu.name = "View";
  viewMenu.menuItems.push_back({"Debugger", [&debugger]() { 
    debugger.open = !debugger.open;
  }});
  mainMenuBar.menus.push_back(viewMenu);

  // Help menu
  Menu helpMenu;
  helpMenu.name = "Help";
  helpMenu.menuItems.push_back({"About", [&showAbout]() { showAbout = true; }});
  mainMenuBar.menus.push_back(helpMenu);

  board->setup(*args);
  
  // Set initial CPU state - break if requested
  if (args->get<bool>("--break")) {
    debugger.cpuPaused = true;
    debugger.open = true;
  }

  while (!WindowShouldClose() && run) {
    BeginDrawing();
    ClearBackground(GRAY);

    float frame_time = GetFrameTime();
    if (frame_time > 5.0) {
      cout << "Warning: Unusual frame time (" << frame_time << "). Did WM lose focus?" << endl;
      frame_time = 1.0 / 60;
    }
    // Only run CPU cycles if not paused
    int cycles_ran = 0;
    if (!debugger.cpuPaused) {
      int target_cycles = static_cast<int>(board->clock_speed * frame_time);
      cycles_ran = board->run(max(1, target_cycles + bias));
      bias = target_cycles - cycles_ran;
      
      // Check if execution was halted
      if (cycles_ran == 0) {
        if (board->cpu) {
          // CPU returned 0 cycles - likely an issue, so pause
          debugger.cpuPaused = true;
        } else {
          // Not a CPU issue, so actually stop
          run = false;
        }
      }
    }
    if (board->display) {
      board->draw();
    }

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "MultiEmu - %.2f MHz - %d cycles - %d FPS", board->clock_speed / 1e6, cycles_ran,
             static_cast<int>(1.0 / frame_time));
    SetWindowTitle(buffer);

    // Draw UI with ImGui
    rlImGuiBegin();

    // Render main menu bar
    mainMenuBar.render();

    // Create a child window that fills the entire screen area below the menu bar
    ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetFrameHeight()));
    ImGui::SetNextWindowSize(ImVec2(GetScreenWidth(), GetScreenHeight() - ImGui::GetFrameHeight()));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    if (ImGui::Begin("BoardOutput", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus |
                         ImGuiWindowFlags_NoNavFocus)) {
      // Draw the board's framebuffer as an ImGui image with preserved aspect ratio
      if (board->display) {
        // Calculate scaling to maintain aspect ratio
        ImVec2 availSize = ImGui::GetContentRegionAvail();
        float texWidth = (float)Display::framebuffer.texture.width;
        float texHeight = (float)Display::framebuffer.texture.height;
        float scale = std::min(availSize.x / texWidth, availSize.y / texHeight);

        ImVec2 imageSize(texWidth * scale, texHeight * scale);

        // Center the image in the available space
        float offsetX = (availSize.x - imageSize.x) * 0.5f;
        float offsetY = (availSize.y - imageSize.y) * 0.5f;
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + offsetX, ImGui::GetCursorPosY() + offsetY));

        // Draw the image with Y-flipped UV coordinates (0,1 → 1,0)
        ImGui::Image((ImTextureID)(intptr_t)Display::framebuffer.texture.id, imageSize, ImVec2(0, 1),
                     ImVec2(1, 0)); // Flipped Y axis
      }
      ImGui::End();
    }
    ImGui::PopStyleVar();

    if (showAbout) {
      ImGui::Begin("About", &showAbout, ImGuiWindowFlags_AlwaysAutoResize);
      ImGui::Text("MultiEmu");
      ImGui::Text("A multi-system emulator framework");
      ImGui::End();
    }

    // Render debugger window if open
    if (debugger.open && board->cpu) {
      debugger.render();
    }

    rlImGuiEnd();

    EndDrawing();
  }

  return 0;
}
