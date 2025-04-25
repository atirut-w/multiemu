#include "ui/window.hpp"
#include "imgui.h"

namespace MultiEmu {

void Window::render() {
  if (ImGui::Begin(title.c_str(), &open, flags)) {
    for (auto &child : children) {
      child->render();
    }
  }
  ImGui::End();
}

}
