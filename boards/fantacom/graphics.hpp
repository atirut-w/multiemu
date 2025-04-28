#pragma once
#include "multiemu/units.hpp"
#include "multiemu/device.hpp"
#include <cstdint>

class Graphics : public MultiEmu::Device {
public:
  uint8_t config[0];
  uint8_t vram[256 * KIB];

  int mode = 0; // Graphics mode

  Graphics() {
    // Initialize VRAM with a pattern
    for (int i = 0; i < 256 * KIB; i++) {
      vram[i] = i;
    }

    modeset();
  }

  // Device interface implementation
  virtual std::string getDeviceType() const override { return "video"; }
  virtual std::string getDeviceName() const override { return "Fantacom VDC"; }
  virtual void initialize() override { modeset(); }
  virtual void reset() override { 
    mode = 0; 
    modeset(); 
  }

  void modeset();
  void draw();

  // Draw in VGA-like 80x25 text mode
  void drawTextMode80x25();
  // Draw in EGA-like 320x200x16 bitmap mode
  void drawBitmapMode320x200x16();
};
