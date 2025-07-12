cmake_minimum_required(VERSION 3.22)
project(MultiEmu)

# Third-party libraries
add_subdirectory(3rd)

add_executable(multiemu
    src/main.cpp
)
target_include_directories(multiemu PRIVATE include)
target_link_libraries(multiemu PRIVATE raylib)
