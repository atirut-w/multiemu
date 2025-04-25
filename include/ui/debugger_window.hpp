#pragma once
#include "imgui.h"
#include "imgui_memory_editor/imgui_memory_editor.h"
#include "multiemu/board.hpp"
#include "multiemu/address_space.hpp"
#include "ui/window.hpp"

namespace MultiEmu {

// Context for memory editor to access the selected address space
struct MemoryEditorContext {
  AddressSpace* addressSpace = nullptr;
};

class DebuggerWindow : public Window {
public:
  Board* board;
  bool cpuPaused;
  MemoryEditor memEdit;
  MemoryEditorContext memEditContext;
  AddressSpace* selectedSpace = nullptr;

  DebuggerWindow(Board* board, bool initialPauseState = false);
  
  virtual void render() override;

private:
  void renderControlButtons();
  void renderRegisters();
  void renderMemoryView();
};

}