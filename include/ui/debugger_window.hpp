#pragma once
#include "imgui.h"
#include "imgui_memory_editor/imgui_memory_editor.h"
#include "multiemu/board.hpp"
#include "ui/window.hpp"
#include <string>
#include <vector>

namespace MultiEmu {

// Forward declaration
class Board;
class Bus;
struct BusInfo;

// Context for memory editor to access the selected bus
struct MemoryEditorContext {
  const BusInfo* selectedBus = nullptr;
};

class DebuggerWindow : public Window {
public:
  Board* board;
  bool cpuPaused;
  MemoryEditor memEdit;
  MemoryEditorContext memEditContext;
  
  // Available buses and currently selected bus
  std::vector<BusInfo> availableBuses;
  const BusInfo* selectedBus = nullptr;

  DebuggerWindow(Board* board, bool initialPauseState = false);
  
  virtual void render() override;

private:
  void renderControlButtons();
  void renderRegisters();
  void renderMemoryView();
  void renderDeviceTree();
  
  // Update available buses from board
  void updateAvailableBuses();
  
  // Helper for rendering the device tree recursively
  void renderDeviceNode(const Device* device);
};

}
