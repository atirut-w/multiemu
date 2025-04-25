#pragma once
#include "imgui.h"
#include <memory>
#include <string>
#include <ui/ui_component.hpp>
#include <vector>

namespace MultiEmu {

class Window : public UIComponent {
public:
  std::string title;
  bool open;
  ImGuiWindowFlags flags;

  std::vector<std::unique_ptr<UIComponent>> children;

  virtual void render() override;
};

}
