#include "multiemu/display.hpp"
#include "raylib.h"

using namespace std;
using namespace MultiEmu;

RenderTexture2D Display::framebuffer;

void Display::init(int width, int height) {
  if (IsRenderTextureValid(framebuffer)) {
    UnloadRenderTexture(framebuffer);
  }
  framebuffer = LoadRenderTexture(width, height);
  SetTextureFilter(framebuffer.texture, TEXTURE_FILTER_POINT);
}

void Display::begin() {
  BeginTextureMode(framebuffer);
  ClearBackground(BLACK);
}

void Display::end() {
  EndTextureMode();
}
