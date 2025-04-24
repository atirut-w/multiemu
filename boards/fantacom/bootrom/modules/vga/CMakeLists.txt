add_library(vga OBJECT
    font.s
    vga.c
)
target_compile_options(vga PRIVATE -I${CMAKE_SOURCE_DIR})
