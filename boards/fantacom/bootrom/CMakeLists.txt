project("fantacom_bootrom" ASM)
configure_file("link.ld" "link.ld" COPYONLY)
add_executable("bootrom" "main.s")
target_link_options("bootrom" PRIVATE "-T" "${CMAKE_CURRENT_BINARY_DIR}/link.ld")

add_custom_target("bootrom.bin" ALL DEPENDS "bootrom"
    COMMENT "Generating bootrom.bin"
    COMMAND "${CMAKE_OBJCOPY}" -O binary "$<TARGET_FILE:bootrom>" "bootrom.bin"
)
