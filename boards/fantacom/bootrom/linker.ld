ENTRY(_start)

MEMORY
{
    ROM (rx) : ORIGIN = 0x0000, LENGTH = 0x4000
    RAM (rwx) : ORIGIN = 0x4000, LENGTH = 0x1000
}

SECTIONS
{
    .text : {
        *(.startup)
        *(.text)
        *(_CODE)
    } > ROM
}
