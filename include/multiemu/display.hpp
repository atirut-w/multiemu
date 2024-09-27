#pragma once
#include "raylib.h"
#include <optional>

namespace MultiEmu {
struct Display {
  static std::optional<RenderTexture2D> framebuffer;

  static void init(int width, int height);
  static void draw();
};
} // namespace MultiEmu
