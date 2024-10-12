    ; Z80 I/O access library

    ; Write a byte to an I/O port
    ; void outb(uint16_t port, uint8_t value);
    .global _outb
    .type _outb, @function
_outb:
    push ix
    ld ix, 0
    add ix, sp

    ld bc, 16
    ld a, 255
    out (c), a

    pop ix
    ret
