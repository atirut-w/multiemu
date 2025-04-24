#include "multiemu/display.hpp"
#include "raylib.h"
#include <algorithm>

using namespace std;
using namespace MultiEmu;

optional<RenderTexture2D> Display::framebuffer;

void Display::init(int width, int height) {
  framebuffer = LoadRenderTexture(width, height);
  SetTextureFilter(framebuffer->texture, TEXTURE_FILTER_POINT);
}

void Display::draw() {
  // Draw the "framebuffer", scaled to the window size with letterboxing
  float scale = min((float)GetScreenWidth() / framebuffer->texture.width,
                    (float)GetScreenHeight() / framebuffer->texture.height);
  Vector2 center = {(float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2};

  Rectangle src = {0, 0, (float)framebuffer->texture.width,
                   -(float)framebuffer->texture.height};
  Rectangle dst = {center.x - framebuffer->texture.width * scale / 2,
                   center.y - framebuffer->texture.height * scale / 2,
                   framebuffer->texture.width * scale,
                   framebuffer->texture.height * scale};

  DrawTexturePro(framebuffer->texture, src, dst, {0, 0}, 0, WHITE);
}
