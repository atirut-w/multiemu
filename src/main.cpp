#include <argparse/argparse.hpp>
#include <iostream>
#include <memory>

using namespace std;
using namespace argparse;

unique_ptr<const ArgumentParser> parse_arguments(int argc, const char *argv[]) {
  auto parser = make_unique<ArgumentParser>("Multi-machine emulator");

  parser->add_argument("-b", "--board").help("Board to emulate").required();

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

  return 0;
}
