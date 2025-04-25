#pragma once
#include "imgui.h"
#include "imgui_memory_editor/imgui_memory_editor.h"
#include "multiemu/board.hpp"
#include "ui/window.hpp"

namespace MultiEmu {

class DebuggerWindow : public Window {
public:
  Board* board;
  bool cpuPaused;
  bool showIOSpace;
  MemoryEditor memEdit;

  DebuggerWindow(Board* board, bool initialPauseState = false);
  
  virtual void render() override;

private:
  void renderControlButtons();
  void renderRegisters();
  void renderMemoryView();
};

}