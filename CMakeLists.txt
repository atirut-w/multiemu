cmake_minimum_required(VERSION 3.22)
project("MultiEmu")
add_subdirectory("3rd/argparse")

set(Z80_WITH_EXECUTE YES)
set(Z80_WITH_FULL_IM0 YES)
set(Z80_WITH_IM0_RETX_NOTIFICATIONS YES)
set(Z80_WITH_Q YES)
set(Z80_WITH_ZILOG_NMOS_LD_A_IR_BUG YES)
set(Z80_SHARED_LIBS NO)
add_subdirectory("3rd/Z80")

add_executable("multiemu"
    "src/main.cpp"
    "src/board_registry.cpp"
)
target_include_directories("multiemu" PRIVATE "include" "boards" "cpus")
target_link_libraries("multiemu" PRIVATE "argparse" "Z80")
install(TARGETS "multiemu" DESTINATION "bin")

add_subdirectory("boards")
add_subdirectory("cpus")
