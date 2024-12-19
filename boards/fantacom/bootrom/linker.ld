ENTRY(_start)

MEMORY
{
    ROM (rx) : ORIGIN = 0x0000, LENGTH = 0x2000
    RAM (rwx) : ORIGIN = 0x2000, LENGTH = 0x2000
}

SECTIONS
{
    .text : {
        *(.text)
        *(_CODE)

        _modules_start = .;
        *(.modules)
        _modules_end = .;

        _initializer_start = .;
        *(_INITIALIZER)
        _initializer_end = .;
    } > ROM

    .data : {
        _data_start = .;
        *(.data)
        *(_DATA)
        _data_end = .;
        _data_load = LOADADDR(.data);

        _initialized_start = .;
        *(_INITIALIZED)
        _initialized_end = .;
    } > RAM AT > ROM

    .bss : {
        _bss_start = .;
        *(.bss)
        _bss_end = .;
    } > RAM
}
