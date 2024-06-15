#include <board_registry.hpp>
#include <cstddef>
#include <stdexcept>

using namespace std;

extern BoardMeta __start_boards;
extern BoardMeta __stop_boards;

// Dummy board to prevent the linker from optimizing out the section
REGISTER_BOARD(DummyBoard, nullptr);

vector<BoardMeta> BoardRegistry::get_boards()
{
    vector<BoardMeta> boards;
    BoardMeta *board = &__start_boards;
    
    while (board < &__stop_boards)
    {
        if (board->ctor)
        {
            boards.push_back(*board);
        }
        board++;
    }

    return boards;
}

BoardMeta BoardRegistry::get_board(const string &name)
{
    BoardMeta *board = &__start_boards;
    
    while (board < &__stop_boards)
    {
        if (board->name == name && board->ctor)
        {
            return *board;
        }
        board++;
    }
    throw invalid_argument("Could not find board \"" + name + "\"");
}
