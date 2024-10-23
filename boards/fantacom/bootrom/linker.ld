ENTRY(_start)

MEMORY
{
    ROM (rx) : ORIGIN = 0x0000, LENGTH = 0x2000
    RAM (rwx) : ORIGIN = 0x2000, LENGTH = 0x1000
}

SECTIONS
{
    .text : {
        *(.startup)
        *(.text)

        drivers_start = .;
        *(.drivers)
        drivers_end = .;
    } > ROM

    .bss : {
        *(.bss)
    } > RAM

    .data : {
        data_start = .;
        *(.data)
        data_end = .;
    } > RAM AT > ROM
}
