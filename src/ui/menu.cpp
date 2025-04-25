#include "ui/menu.hpp"
#include "imgui.h"

namespace MultiEmu {

void Menu::render() const {
  if (ImGui::BeginMenu(name.c_str())) {
    for (const auto &menuItem : menuItems) {
      if (ImGui::MenuItem(menuItem.name.c_str())) {
        menuItem.action();
      }
    }
    ImGui::EndMenu();
  }
}

} // namespace MultiEmu
