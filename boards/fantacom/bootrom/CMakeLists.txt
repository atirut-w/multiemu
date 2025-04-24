project(bootrom C ASM)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(boot)
add_subdirectory(crt)
add_subdirectory(libgcc)
add_subdirectory(modules)

add_executable(bootrom
    src/main.c
)
target_compile_options(bootrom PRIVATE -nostdlib -Oz)
target_link_options(bootrom PRIVATE -T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld -nostdlib)
target_link_libraries(bootrom PRIVATE
    boot
    crt 
    gcc
    vga
)

add_custom_target(
    bootrom.bin ALL
    DEPENDS bootrom
    COMMAND ${CMAKE_OBJCOPY} -O binary ${CMAKE_CURRENT_BINARY_DIR}/bootrom ${CMAKE_CURRENT_BINARY_DIR}/bootrom.bin
)
