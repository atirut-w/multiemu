#include "ui/debugger_window.hpp"
#include "imgui.h"
#include "multiemu/bus.hpp"
#include <map>

namespace MultiEmu {

// Memory read/write callbacks for the memory editor
static ImU8 MemoryReadFn(const ImU8* mem, size_t addr, void* user_data) {
  MemoryEditorContext* context = static_cast<MemoryEditorContext*>(user_data);
  if (!context || !context->selectedBus || !context->selectedBus->bus) {
    return 0;
  }

  const BusInfo& busInfo = *context->selectedBus;
  
  // Simply call the bus read function - no need for type dispatch anymore
  return busInfo.bus->read(static_cast<uint32_t>(addr));
}

static void MemoryWriteFn(ImU8* mem, size_t addr, ImU8 value, void* user_data) {
  MemoryEditorContext* context = static_cast<MemoryEditorContext*>(user_data);
  if (!context || !context->selectedBus || !context->selectedBus->bus) {
    return;
  }

  const BusInfo& busInfo = *context->selectedBus;
  
  // Simply call the bus write function - no need for type dispatch anymore
  busInfo.bus->write(static_cast<uint32_t>(addr), value);
}

// Highlight the current program counter
static bool MemoryHighlightFn(const ImU8* mem, size_t addr, void* user_data) {
  MemoryEditorContext* context = static_cast<MemoryEditorContext*>(user_data);
  if (context && context->selectedBus && context->selectedBus->getProgramCounter) {
    return addr == context->selectedBus->getProgramCounter();
  }
  return false;
}

DebuggerWindow::DebuggerWindow(Board* board, bool initialPauseState) 
    : board(board), cpuPaused(initialPauseState) {
  title = "Debugger";
  open = false;
  flags = 0;
  
  // Initialize memory editor
  memEdit.ReadOnly = false;
  memEdit.ReadFn = MemoryReadFn;
  memEdit.WriteFn = MemoryWriteFn;
  memEdit.HighlightFn = MemoryHighlightFn;
  memEdit.UserData = &memEditContext;  // Pass context to callbacks
  memEdit.OptShowDataPreview = true;
  memEdit.OptShowOptions = true;
  memEdit.OptUpperCaseHex = true;
  
  // Initialize buses
  updateAvailableBuses();
}

void DebuggerWindow::render() {
  if (ImGui::Begin(title.c_str(), &open)) {
    renderControlButtons();
    renderRegisters();
    renderMemoryView();
  }
  ImGui::End();
}

void DebuggerWindow::updateAvailableBuses() {
  // Clear old buses
  availableBuses.clear();
  selectedBus = nullptr;
  memEditContext.selectedBus = nullptr;
  
  // Get new buses from board
  if (board) {
    availableBuses = board->get_buses();
    
    // Select the first bus by default
    if (!availableBuses.empty()) {
      selectedBus = &availableBuses[0];
      memEditContext.selectedBus = selectedBus;
    }
  }
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
    board->getCPU()->run(1);
    
    // Restore paused state (usually keeping it paused)
    cpuPaused = wasPaused || true;
  }

  ImGui::SameLine();
  if (ImGui::Button("Reset")) {
    board->getCPU()->reset();
  }
}

void DebuggerWindow::renderRegisters() {
  if (ImGui::CollapsingHeader("Registers", ImGuiTreeNodeFlags_DefaultOpen)) {
    // Get CPU metadata and current register values
    auto registers = board->getCPU()->getAllRegisters();
    auto regInfo = board->getCPU()->getRegisterInfo();
    auto regGroups = board->getCPU()->getRegisterGroups();
    auto flags = board->getCPU()->getFlagDefinitions();
    auto capabilities = board->getCPU()->getDebuggerCapabilities();

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
    // Make sure buses are up to date
    if (availableBuses.empty()) {
      updateAvailableBuses();
    }
    
    // Check if we have any buses to show
    if (availableBuses.empty()) {
      ImGui::Text("No buses available for debugging.");
    } else {
      // Show bus selector
      if (ImGui::BeginCombo("Bus", selectedBus ? selectedBus->name.c_str() : "None")) {
        for (size_t i = 0; i < availableBuses.size(); i++) {
          bool isSelected = (selectedBus == &availableBuses[i]);
          if (ImGui::Selectable(availableBuses[i].name.c_str(), isSelected)) {
            selectedBus = &availableBuses[i];
            memEditContext.selectedBus = selectedBus;
            // Reset goto address when switching buses
            memEdit.GotoAddr = 0;
          }
          if (isSelected) {
            ImGui::SetItemDefaultFocus();
          }
        }
        ImGui::EndCombo();
      }
      
      // Show memory editor if a bus is selected
      if (selectedBus) {
        // Get the max address for the bus's width
        size_t maxSize = selectedBus->bus->getMaxAddress() + 1;
        memEdit.DrawContents(nullptr, maxSize, 0);
      }
    }
  }
}

} // namespace MultiEmu
