cmake_minimum_required(VERSION 3.22)
project("MultiEmu")
add_subdirectory("3rd/argparse")

add_executable("multiemu"
    "src/main.cpp"
    "src/board_registry.cpp"
)
target_include_directories("multiemu" PRIVATE "include" "boards" "cpus")
target_link_libraries("multiemu" PRIVATE "argparse")
install(TARGETS "multiemu" DESTINATION "bin")

add_subdirectory("boards")
add_subdirectory("cpus")
