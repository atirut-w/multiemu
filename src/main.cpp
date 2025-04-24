#include "imgui.h"
#include "imgui_memory_editor/imgui_memory_editor.h"
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

// Global variable for memory editor callbacks
static bool g_showIOSpace = false;

// Memory read/write callbacks for the memory editor
static ImU8 MemoryReadFn(const ImU8* mem, size_t addr, void* user_data) {
  Board* board = static_cast<Board*>(user_data);
  if (board && board->cpu) {
    if (board->cpu->getDebuggerCapabilities().supportsPortIO && g_showIOSpace) {
      return board->cpu->in(addr);
    } else {
      return board->cpu->read(addr);
    }
  }
  return 0;
}

static void MemoryWriteFn(ImU8* mem, size_t addr, ImU8 value, void* user_data) {
  Board* board = static_cast<Board*>(user_data);
  if (board && board->cpu) {
    if (board->cpu->getDebuggerCapabilities().supportsPortIO && g_showIOSpace) {
      board->cpu->out(addr, value);
    } else {
      board->cpu->write(addr, value);
    }
  }
}

// Highlight the current program counter
static bool MemoryHighlightFn(const ImU8* mem, size_t addr, void* user_data) {
  Board* board = static_cast<Board*>(user_data);
  if (board && board->cpu) {
    // Only highlight PC in normal memory view, not in I/O space view
    if (!g_showIOSpace) {
      return addr == board->cpu->getProgramCounter();
    }
  }
  return false;
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
  InitWindow(1280, 720, "MultiEmu - Initializing...");
  SetTargetFPS(60);
  rlImGuiSetup(true); // Dark theme my beloved
  bool run = true;
  bool showAbout = false;
  bool showDebugger = false;
  bool cpuPaused = false;
  int bias = 0;

  // Debugger state
  g_showIOSpace = false;
  MemoryEditor memEdit;
  
  // Initialize memory editor
  memEdit.ReadOnly = false;
  memEdit.ReadFn = MemoryReadFn;
  memEdit.WriteFn = MemoryWriteFn;
  memEdit.HighlightFn = MemoryHighlightFn;
  memEdit.UserData = board.get();  // Pass board pointer to callbacks
  memEdit.OptShowDataPreview = true;
  memEdit.OptShowOptions = true;
  memEdit.OptUpperCaseHex = true;

  board->setup(*args);
  if (board->display) {
    Display::init(640, 400);
  }
  
  // Set initial CPU state - break if requested
  if (args->get<bool>("--break")) {
    cpuPaused = true;
    showDebugger = true;
    // Make sure CPU stops immediately after first instruction
    if (board->cpu) {
      board->cpu->stop();
    }
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
    if (!cpuPaused) {
      int target_cycles = static_cast<int>(board->clock_speed * frame_time);
      cycles_ran = board->run(max(1, target_cycles + bias));
      bias = target_cycles - cycles_ran;
      
      // Check if execution was halted
      if (cycles_ran == 0) {
        if (board->cpu) {
          // Execution halted - pause the CPU
          cpuPaused = true;
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

    if (ImGui::BeginMainMenuBar()) {
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Quit")) {
          run = false;
        }
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("View")) {
        if (ImGui::MenuItem("Debugger")) {
          showDebugger = !showDebugger;
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

    if (showDebugger && board->cpu) {
      ImGui::Begin("Debugger", &showDebugger);

      // Control buttons
      if (ImGui::Button(cpuPaused ? "Resume" : "Pause")) {
        cpuPaused = !cpuPaused;
        if (cpuPaused) {
          // Pause CPU
          board->cpu->stop();
        } else {
          // Resume CPU - just clear the halted state without resetting
          board->cpu->resume();
        }
      }

      ImGui::SameLine();
      if (ImGui::Button("Step")) {
        // Make sure CPU is paused first
        if (!cpuPaused) {
          cpuPaused = true;
          board->cpu->stop();
        }
        
        // Temporarily clear the halted state, execute one instruction, then re-halt
        board->cpu->resume();  // Clear halted state
        board->cpu->execute(1); // Execute a single instruction
        board->cpu->stop();     // Re-halt the CPU
      }

      ImGui::SameLine();
      if (ImGui::Button("Reset")) {
        board->cpu->reset();
      }

      // Register display - CPU-agnostic implementation
      if (ImGui::CollapsingHeader("Registers", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Get CPU metadata and current register values
        auto registers = board->cpu->getAllRegisters();
        auto regInfo = board->cpu->getRegisterInfo();
        auto regGroups = board->cpu->getRegisterGroups();
        auto flags = board->cpu->getFlagDefinitions();
        auto capabilities = board->cpu->getDebuggerCapabilities();

        // Create mapping of register name to info for quick lookup
        std::map<std::string, RegisterInfo> regInfoMap;
        for (const auto &info : regInfo) {
          regInfoMap[info.name] = info;
        }

        // Display registers by groups
        if (!regGroups.empty()) {
          // Use CPU-provided grouping
          for (const auto &group : regGroups) {
            if (ImGui::TreeNodeEx(group.name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
              ImGui::Columns(4, ("regs_" + group.name).c_str());

              for (const auto &regName : group.registerNames) {
                if (registers.count(regName)) {
                  uint64_t value = registers[regName];

                  // Check if we have metadata for this register
                  if (regInfoMap.count(regName)) {
                    const auto &info = regInfoMap[regName];

                    // Format based on register size and preferred format
                    if (info.displayFormat == "hex") {
                      if (info.bitWidth <= 8) {
                        ImGui::Text("%s: %02X", regName.c_str(), (uint8_t)value);
                      } else if (info.bitWidth <= 16) {
                        ImGui::Text("%s: %04X", regName.c_str(), (uint16_t)value);
                      } else {
                        ImGui::Text("%s: %08X", regName.c_str(), (uint32_t)value);
                      }
                    } else if (info.displayFormat == "dec") {
                      ImGui::Text("%s: %d", regName.c_str(), (int)value);
                    } else if (info.displayFormat == "bin") {
                      // Binary format is more complex - simplified here
                      ImGui::Text("%s: %d", regName.c_str(), (int)value);
                    }

                    // Show tooltip with description on hover
                    if (!info.description.empty() && ImGui::IsItemHovered()) {
                      ImGui::SetTooltip("%s", info.description.c_str());
                    }
                  } else {
                    // Fallback formatting if no metadata
                    if (value <= 0xFF) {
                      ImGui::Text("%s: %02X", regName.c_str(), (uint8_t)value);
                    } else if (value <= 0xFFFF) {
                      ImGui::Text("%s: %04X", regName.c_str(), (uint16_t)value);
                    } else {
                      ImGui::Text("%s: %08X", regName.c_str(), (uint32_t)value);
                    }
                  }

                  ImGui::NextColumn();
                }
              }

              ImGui::Columns(1);
              ImGui::TreePop();
            }
          }
        } else {
          // Fallback if no groups defined
          ImGui::Columns(4, "registers");

          for (const auto &[name, value] : registers) {
            if (value <= 0xFF) {
              ImGui::Text("%s: %02X", name.c_str(), (uint8_t)value);
            } else if (value <= 0xFFFF) {
              ImGui::Text("%s: %04X", name.c_str(), (uint16_t)value);
            } else {
              ImGui::Text("%s: %08X", name.c_str(), (uint32_t)value);
            }
            ImGui::NextColumn();
          }

          ImGui::Columns(1);
        }

        // Flag register display (using metadata if available)
        if (capabilities.hasStatusRegister && registers.count(capabilities.statusRegisterName) && !flags.empty()) {
          uint8_t flagReg = (uint8_t)registers[capabilities.statusRegisterName];

          ImGui::Text("Flags: ");
          ImGui::SameLine();

          // Display flag bits based on metadata
          for (const auto &flag : flags) {
            bool isSet = (flagReg & (1 << flag.bitPosition)) != 0;

            if (isSet) {
              ImGui::TextColored(ImVec4(0, 1, 0, 1), "%c", flag.shortName);
            } else {
              ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1), "%c", '-');
            }
            ImGui::SameLine();

            // Show tooltip with description on hover
            if (ImGui::IsItemHovered()) {
              ImGui::SetTooltip("%s (bit %d): %s", flag.name.c_str(), flag.bitPosition, flag.description.c_str());
            }
          }
          ImGui::NewLine();
        }
      }

      // Memory view using ImGui memory editor
      if (ImGui::CollapsingHeader("Memory", ImGuiTreeNodeFlags_DefaultOpen)) {
        auto capabilities = board->cpu->getDebuggerCapabilities();
        
        // I/O Space toggle if supported
        if (capabilities.supportsPortIO) {
          if (ImGui::Checkbox("I/O Space", &g_showIOSpace)) {
            // Update global variable for memory editor callbacks
            if (g_showIOSpace) {
              memEdit.GotoAddr = 0; // Reset memory view when switching to I/O
            }
          }
        }
        
        // Set max address based on CPU capabilities
        size_t maxAddress = capabilities.maxAddressSpace > 0 ? capabilities.maxAddressSpace : 0xFFFF;
        
        // Display the memory editor
        memEdit.DrawContents(nullptr, maxAddress + 1, 0);
      }

      ImGui::End();
    }

    rlImGuiEnd();

    EndDrawing();
  }

  return 0;
}
