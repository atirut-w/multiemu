cmake_minimum_required(VERSION 3.22)
project("MultiEmu")
add_subdirectory("3rd/argparse")

add_executable("multiemu"
    "src/main.cpp"
)
target_link_directories("multiemu" PRIVATE "include")
target_link_libraries("multiemu" PRIVATE "argparse")
