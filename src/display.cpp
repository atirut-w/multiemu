#include "multiemu/display.hpp"
#include "raylib.h"

using namespace std;
using namespace MultiEmu;

optional<RenderTexture2D> Display::framebuffer;

void Display::init(int width, int height) {
  framebuffer = LoadRenderTexture(width, height);
  SetTextureFilter(framebuffer->texture, TEXTURE_FILTER_POINT);
}
