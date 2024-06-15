#include <iostream>
#include <argparse/argparse.hpp>
#include <memory>

using namespace std;
using namespace argparse;

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

    return 0;
}
