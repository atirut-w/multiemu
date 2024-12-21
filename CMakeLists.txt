cmake_minimum_required(VERSION 3.22)
project(MultiEmu)
set(GLFW_BUILD_WAYLAND OFF)
add_subdirectory(3rd/argparse)
add_subdirectory(3rd/raylib)

add_executable(multiemu
    src/main.cpp
    src/board_registry.cpp
    src/display.cpp
    src/utils.cpp
)
target_include_directories(multiemu PRIVATE include boards cpus 3rd/z80)
target_link_libraries(multiemu PRIVATE argparse raylib)
install(TARGETS multiemu DESTINATION bin)

add_subdirectory(boards)
add_subdirectory(cpus)
