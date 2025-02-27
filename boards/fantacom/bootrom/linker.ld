MEMORY {
    ROM (rx) : ORIGIN = 0x0000, LENGTH = 8k
    RAM (rw) : ORIGIN = 0x2000, LENGTH = 8k
}

SECTIONS {
    .text : {
        KEEP(*(.vectors))
        *(.text)
    } > ROM

    .data : {
        *(.data)
    } > RAM AT > ROM

    .bss : {
        *(.bss)
    } > RAM
}
