#include "multiemu/device.hpp"

namespace MultiEmu {

void Device::addChild(std::unique_ptr<Device> device) {
  Device* devicePtr = device.get(); // Keep raw pointer before transfer
  children.push_back(std::move(device)); // Transfer ownership
  devicePtr->setParent(this);
}

const std::vector<std::unique_ptr<Device>>& Device::getChildren() const {
  return children;
}

Device* Device::getParent() const {
  return parent;
}

void Device::setParent(Device* parent) {
  this->parent = parent;
}

} // namespace MultiEmu