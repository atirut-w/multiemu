#include <argparse/argparse.hpp>
#include <board_registry.hpp>
#include <iostream>
#include <memory>
#include <multiemu/board.hpp>

using namespace std;
using namespace argparse;

unique_ptr<const ArgumentParser> parse_arguments(int argc, const char *argv[]) {
  auto parser = make_unique<ArgumentParser>("Multi-machine emulator");

  auto &mutex = parser->add_mutually_exclusive_group(true);

  // Board to emulate
  mutex.add_argument("board").help("Board to emulate").nargs(1);

  // List available boards
  mutex.add_argument("-l", "--list").help("List available boards").default_value(false).implicit_value(true);

  try {
    parser->parse_args(argc, argv);
    return parser;
  } catch (const exception &error) {
    cerr << error.what() << endl;
    cerr << *parser;
    return nullptr;
  }
}

int main(int argc, const char *argv[]) {
  auto args = parse_arguments(argc, argv);
  if (!args) {
    return 1;
  }

  if (args->get<bool>("--list")) {
    cout << "Available boards:" << endl;
    for (const auto *board : BoardRegistry::get_board_infos()) {
      cout << "\t" << board->name << endl;
    }

    return 0;
  }

  auto board = BoardRegistry::create_board(args->get<string>("board"));
  if (!board) {
    cerr << "Board not found: " << args->get<string>("board") << endl;
    return 1;
  }

  while (true) {
    board->step();
  }

  return 0;
}
