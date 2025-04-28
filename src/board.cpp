#include "multiemu/board.hpp"

namespace MultiEmu {

/**
 * Find the primary CPU in the device hierarchy
 * This method ensures backward compatibility with code that expects
 * boards to have a direct CPU reference.
 */
CPU* Board::getCPU() {
  // First check direct children
  for (const auto& child : children) {
    if (child->getDeviceType() == "cpu") {
      return static_cast<CPU*>(child.get());
    }
  }
  
  // If no direct CPU child was found, recursively check all children
  for (const auto& child : children) {
    for (const auto& grandchild : child->getChildren()) {
      if (grandchild->getDeviceType() == "cpu") {
        return static_cast<CPU*>(grandchild.get());
      }
    }
  }
  
  // No CPU found in the hierarchy
  return nullptr;
}

} // namespace MultiEmu