#pragma once
#include "argparse/argparse.hpp"
#include "multiemu/board.hpp"
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

namespace MultiEmu {

class BoardRegistry {
public:
  // Board information structure
  struct BoardInfo {
    std::string description;
    std::function<std::unique_ptr<Board>()> factory;
  };
  
private:
  // Map of board names to their info
  static std::unordered_map<std::string, BoardInfo> board_infos;

public:
  /**
   * Get a read-only reference to all board infos
   * @return Const reference to the board info map
   */
  static const std::unordered_map<std::string, BoardInfo>& get_board_infos();
  
  /**
   * Get a list of all board type names
   * @return Vector of board names
   */
  static std::vector<std::string> get_board_names();
  
  /**
   * Create a new board instance by name
   * @param name The name of the board to create
   * @return Unique pointer to the new board instance, or nullptr if not found
   */
  static std::unique_ptr<Board> create_board(const std::string &name);

  /**
   * Helper class for automatic board registration
   */
  template <class BoardType> class Register {
  public:
    Register(const std::string &name, const std::string &description = "") {
      // Register the board info with factory, replacing any existing registration
      board_infos[name] = {
        description,
        []() { return std::make_unique<BoardType>(); }
      };
    }
  };
};

} // namespace MultiEmu
