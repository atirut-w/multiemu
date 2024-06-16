#include <board_registry.hpp>
#include <cstddef>
#include <stdexcept>
#include <iostream>

using namespace std;

extern const BoardMeta __start_boards;
extern const BoardMeta __stop_boards;

// Dummy board to prevent the linker from optimizing out the section
REGISTER_BOARD(DummyBoard, nullptr);

const vector<const BoardMeta *> BoardRegistry::get_board_metas()
{
    vector<const BoardMeta *> boards;
    const BoardMeta *board = &__start_boards;
    
    while (board < &__stop_boards)
    {
        if (board->ctor_wrapper)
        {
            boards.push_back(board);
        }
        board++;
    }

    return boards;
}

unique_ptr<Board> BoardRegistry::get_board(const string &name)
{
    const BoardMeta *board = &__start_boards;
    
    while (board < &__stop_boards)
    {
        if (board->name == name && board->ctor_wrapper)
        {
            cout << "Instantiating board \"" << name << "\"" << endl;
            return board->ctor_wrapper();
        }
        board++;
    }
    throw invalid_argument("Board \"" + name + "\" not found");
}
