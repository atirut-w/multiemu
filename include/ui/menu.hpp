#pragma once
#include "ui/ui_component.hpp"
#include <functional>
#include <string>
#include <vector>

namespace MultiEmu {

class Menu : public UIComponent {
public:
  struct MenuItem {
    std::string name;
    std::function<void()> action;
  };

  std::string name;
  std::vector<MenuItem> menuItems;

  virtual void render() override;
};

}
