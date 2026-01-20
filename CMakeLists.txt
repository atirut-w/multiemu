cmake_minimum_required(VERSION 3.22)
project(MultiEmu)

add_executable(multiemu 
    src/main.cpp
)
target_include_directories(multiemu PRIVATE include)
