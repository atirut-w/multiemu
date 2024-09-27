ENTRY(_start)

MEMORY
{
    ROM (rx) : ORIGIN = 0x0000, LENGTH = 0x4000
}

SECTIONS
{
    .text : {
        *(.text._start)
        *(.text)
    } > ROM
}
