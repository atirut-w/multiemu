#include <iostream>
#include <argparse/argparse.hpp>
#include <memory>
#include <board.hpp>
#include <board_registry.hpp>

using namespace std;
using namespace argparse;

unique_ptr<const ArgumentParser> parse_arguments(int argc, const char *argv[])
{
    auto parser = make_unique<ArgumentParser>("Multi-machine emulator");

    parser->add_argument("-b", "--board")
        .help("Board to emulate")
        .required();

    try
    {
        parser->parse_args(argc, argv);
        return parser;
    }
    catch (const exception &error)
    {
        cerr << error.what() << endl;
        cerr << *parser;
        exit(1);
    }
}

int main(int argc, const char *argv[])
{
    auto args = parse_arguments(argc, argv);
    
    unique_ptr<Board> board;
    for (auto &meta : BoardRegistry::get_boards())
    {
        if (meta.name == args->get<string>("--board"))
        {
            board = meta.ctor();
            break;
        }
    }
    if (!board)
    {
        cerr << "Could not find board \"" << args->get<string>("--board") << "\"" << endl;
        exit(1);
    }

    return 0;
}
