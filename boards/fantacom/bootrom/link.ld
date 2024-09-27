ENTRY(_start)

MEMORY
{
    RST (rx) : ORIGIN = 0x0000, LENGTH = 0x0040
    ROM (rx) : ORIGIN = 0x0040, LENGTH = 0x4000 - 0x0040
}

SECTIONS
{
    .text.rst : {
        *(.text.rst)
    } > RST
    
    .text : {
        *(.text)
    } > ROM
}
