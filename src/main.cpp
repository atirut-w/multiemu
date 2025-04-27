#include <raylib.h>

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "MultiEmu");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        EndDrawing();
    }

    CloseWindow(); // Close window and OpenGL context
}
