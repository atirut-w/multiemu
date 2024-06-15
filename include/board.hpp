#pragma once
#include <string>
#include <memory>

struct Board
{
    virtual ~Board() = default;

    std::string description;
};

struct BoardMeta
{
    const char *name;
    Board *(*ctor)();
};

#define REGISTER_BOARD(name, ctor) __attribute__((section("boards"))) BoardMeta name##_meta = {#name, ctor};
