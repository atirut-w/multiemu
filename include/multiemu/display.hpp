#pragma once
#include "raylib.h"
#include <optional>

namespace MultiEmu {
class Display {
public:
  static RenderTexture2D framebuffer;

  static void init(int width, int height);
  static void begin();
  static void end();
};
} // namespace MultiEmu
