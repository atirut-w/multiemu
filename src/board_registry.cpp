#include "multiemu/board.hpp"
#include <multiemu/board_registry.hpp>

using namespace std;
using namespace argparse;
using namespace MultiEmu;

unordered_map<string, BoardRegistry::BoardInfo> BoardRegistry::board_infos;

const unordered_map<string, BoardRegistry::BoardInfo>& BoardRegistry::get_board_infos() {
  return board_infos;
}

vector<string> BoardRegistry::get_board_names() {
  vector<string> names;
  names.reserve(board_infos.size());
  
  for (const auto& [name, _] : board_infos) {
    names.push_back(name);
  }
  return names;
}

unique_ptr<Board> BoardRegistry::create_board(const string &name) {
  auto it = board_infos.find(name);
  if (it != board_infos.end()) {
    return it->second.factory();
  }
  return nullptr;
}
