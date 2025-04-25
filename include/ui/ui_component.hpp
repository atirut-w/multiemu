#pragma once

namespace MultiEmu {

class UIComponent {
public:
  virtual ~UIComponent() = default;

  // Render the UI component
  virtual void render() const = 0;
};

}
