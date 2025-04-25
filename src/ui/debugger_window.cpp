#include "ui/debugger_window.hpp"
#include "imgui.h"
#include <map>

namespace MultiEmu {

// Memory read/write callbacks for the memory editor
static ImU8 MemoryReadFn(const ImU8* mem, size_t addr, void* user_data) {
  DebuggerWindow* debugger = static_cast<DebuggerWindow*>(user_data);
  if (debugger && debugger->board && debugger->board->cpu) {
    if (debugger->board->cpu->getDebuggerCapabilities().supportsPortIO && debugger->showIOSpace) {
      return debugger->board->cpu->in(addr);
    } else {
      return debugger->board->cpu->read(addr);
    }
  }
  return 0;
}

static void MemoryWriteFn(ImU8* mem, size_t addr, ImU8 value, void* user_data) {
  DebuggerWindow* debugger = static_cast<DebuggerWindow*>(user_data);
  if (debugger && debugger->board && debugger->board->cpu) {
    if (debugger->board->cpu->getDebuggerCapabilities().supportsPortIO && debugger->showIOSpace) {
      debugger->board->cpu->out(addr, value);
    } else {
      debugger->board->cpu->write(addr, value);
    }
  }
}

// Highlight the current program counter
static bool MemoryHighlightFn(const ImU8* mem, size_t addr, void* user_data) {
  DebuggerWindow* debugger = static_cast<DebuggerWindow*>(user_data);
  if (debugger && debugger->board && debugger->board->cpu) {
    // Only highlight PC in normal memory view, not in I/O space view
    if (!debugger->showIOSpace) {
      return addr == debugger->board->cpu->getProgramCounter();
    }
  }
  return false;
}

DebuggerWindow::DebuggerWindow(Board* board, bool initialPauseState) 
    : board(board), cpuPaused(initialPauseState), showIOSpace(false) {
  title = "Debugger";
  open = true;
  flags = 0;
  
  // Initialize memory editor
  memEdit.ReadOnly = false;
  memEdit.ReadFn = MemoryReadFn;
  memEdit.WriteFn = MemoryWriteFn;
  memEdit.HighlightFn = MemoryHighlightFn;
  memEdit.UserData = this;  // Pass this pointer to callbacks
  memEdit.OptShowDataPreview = true;
  memEdit.OptShowOptions = true;
  memEdit.OptUpperCaseHex = true;
}

void DebuggerWindow::render() {
  if (ImGui::Begin(title.c_str(), &open)) {
    renderControlButtons();
    renderRegisters();
    renderMemoryView();
  }
  ImGui::End();
}

void DebuggerWindow::renderControlButtons() {
  if (ImGui::Button(cpuPaused ? "Resume" : "Pause")) {
    cpuPaused = !cpuPaused;
  }

  ImGui::SameLine();
  if (ImGui::Button("Step")) {
    // Make sure CPU is paused
    bool wasPaused = cpuPaused;
    cpuPaused = false;
    
    // Execute a single instruction
    board->cpu->execute(1);
    
    // Restore paused state (usually keeping it paused)
    cpuPaused = wasPaused || true;
  }

  ImGui::SameLine();
  if (ImGui::Button("Reset")) {
    board->cpu->reset();
  }
}

void DebuggerWindow::renderRegisters() {
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
}

void DebuggerWindow::renderMemoryView() {
  if (ImGui::CollapsingHeader("Memory", ImGuiTreeNodeFlags_DefaultOpen)) {
    auto capabilities = board->cpu->getDebuggerCapabilities();
    
    // I/O Space toggle if supported
    if (capabilities.supportsPortIO) {
      if (ImGui::Checkbox("I/O Space", &showIOSpace)) {
        // Reset memory view when switching to I/O
        if (showIOSpace) {
          memEdit.GotoAddr = 0;
        }
      }
    }
    
    // Set max address based on CPU capabilities
    size_t maxAddress = capabilities.maxAddressSpace > 0 ? capabilities.maxAddressSpace : 0xFFFF;
    
    // Display the memory editor
    memEdit.DrawContents(nullptr, maxAddress + 1, 0);
  }
}

} // namespace MultiEmu
