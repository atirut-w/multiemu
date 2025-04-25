#pragma once
#include "ui/ui_component.hpp"
#include "ui/menu.hpp"

namespace MultiEmu {

class MainMenuBar : public UIComponent {
public:
  std::vector<Menu> menus;

  virtual void render() override;
};

}
