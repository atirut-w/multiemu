#include <iostream>
#include <argparse/argparse.hpp>
#include <memory>
#include <board.hpp>
#include <board_registry.hpp>

using namespace std;
using namespace argparse;

struct MyBoard : public Board
{
};

unique_ptr<Board> my_board_ctor()
{
    
    return make_unique<MyBoard>();
}

struct MyOtherBoard : public Board
{
};

unique_ptr<Board> my_other_board_ctor()
{
    
    return make_unique<MyOtherBoard>();
}

REGISTER_BOARD(MyBoard, my_board_ctor);
REGISTER_BOARD(MyOtherBoard, my_other_board_ctor);

unique_ptr<const ArgumentParser> parse_arguments(int argc, const char *argv[])
{
    auto parser = make_unique<ArgumentParser>("Multi-machine emulator");

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
    
    for (auto &meta : BoardRegistry::get_boards())
    {
        cout << meta.name << endl;
    }

    return 0;
}
