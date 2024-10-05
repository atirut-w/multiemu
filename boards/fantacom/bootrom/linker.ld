ENTRY(_start)

MEMORY
{
    ROM (rx) : ORIGIN = 0x0040, LENGTH = 0x4000
}

SECTIONS
{
    .text : {
        *(.text)
        *(_CODE)
    } > ROM
}
