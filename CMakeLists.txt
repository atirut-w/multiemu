cmake_minimum_required(VERSION 3.22)
project(MultiEmu)
set(GLFW_BUILD_WAYLAND OFF)

add_subdirectory(3rd/argparse)
add_subdirectory(3rd/raylib)

add_library(ImGui STATIC
    3rd/imgui/imgui.cpp
    3rd/imgui/imgui_draw.cpp
    3rd/imgui/imgui_widgets.cpp
    3rd/imgui/imgui_tables.cpp
)
target_include_directories(ImGui PUBLIC 3rd/imgui)

add_library(rlImGui STATIC
    3rd/rlImGui/rlImGui.cpp
)
target_include_directories(rlImGui PUBLIC 3rd/rlImGui)
target_link_libraries(rlImGui PRIVATE raylib ImGui)

add_executable(multiemu
    src/main.cpp
    src/board_registry.cpp
    src/display.cpp
    src/utils.cpp
    src/memory_region.cpp

    src/ui/main_menu_bar.cpp
    src/ui/menu.cpp
    src/ui/window.cpp

    src/cpu/z80.cpp
)
target_include_directories(multiemu PRIVATE include boards cpus 3rd/z80 3rd/imgui_club)
target_link_libraries(multiemu PRIVATE argparse raylib rlImGui ImGui)
install(TARGETS multiemu DESTINATION bin)

add_subdirectory(boards)
add_subdirectory(cpus)
