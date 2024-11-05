ENTRY(_start)

MEMORY
{
    ROM (rx) : ORIGIN = 0x0000, LENGTH = 0x2000
    RAM (rwx) : ORIGIN = 0x2000, LENGTH = 0x2000
}

SECTIONS
{
    .text : {
        *(.startup)
        *(.text)
        *(_CODE)

        _drivers_start = .;
        *(.drivers)
        _drivers_end = .;
    } > ROM

    .data : {
        _data_start = .;
        *(.data)
        *(_DATA)
        _data_end = .;
    } > RAM AT > ROM
    _data_load = LOADADDR(.data);

    .bss : {
        _bss_start = .;
        *(.bss)
        _bss_end = .;
    } > RAM
}
