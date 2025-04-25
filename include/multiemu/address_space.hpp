#pragma once
#include <cstdint>
#include <functional>
#include <memory>
#include <string>

namespace MultiEmu {

enum class AddressSpaceType {
  MEMORY, // Default unified memory (von Neumann)
  IO,     // I/O space
  CUSTOM  // For any other address spaces
};

struct AddressSpace {
  std::string name;
  AddressSpaceType type;
  size_t size;

  // Callbacks for memory editor integration
  std::function<uint8_t(size_t)> read;
  std::function<void(size_t, uint8_t)> write;

  // Optional PC highlight (only applies to program memory spaces)
  std::function<size_t()> getProgramCounter = nullptr;

  AddressSpace(const std::string &name, AddressSpaceType type, size_t size, std::function<uint8_t(size_t)> read,
               std::function<void(size_t, uint8_t)> write, std::function<size_t()> getProgramCounter = nullptr)
      : name(name), type(type), size(size), read(read), write(write), getProgramCounter(getProgramCounter) {}
};

} // namespace MultiEmu
