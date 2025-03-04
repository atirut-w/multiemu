MEMORY {
    ROM (rx) : ORIGIN = 0x0000, LENGTH = 8k
    RAM (rw) : ORIGIN = 0x2000, LENGTH = 8k
}

SECTIONS {
    .text : {
        KEEP(*(.vectors))
        *(.text)
    } > ROM

    .rodata : {
        *(.rodata)

        ___init_array_start = .;
        KEEP(*(.init_array))
        ___init_array_end = .;
    } > ROM

    .data : {
        ___data_start = .;
        *(.data)
        ___data_end = .;
        ___data_load = LOADADDR(.data);
    } > RAM AT > ROM

    .bss : {
        ___bss_start = .;
        *(.bss)
        ___bss_end = .;
    } > RAM
}
