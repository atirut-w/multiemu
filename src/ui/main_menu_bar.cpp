#include "ui/main_menu_bar.hpp"
#include "imgui.h"

using namespace MultiEmu;

void MainMenuBar::render() {
  if (ImGui::BeginMainMenuBar()) {
    for (const auto &menu : menus) {
      menu.render();
    }
    ImGui::EndMainMenuBar();
  }
}
