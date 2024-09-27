    .global _start
    .type _start, @function
_start:
    ld a, 4
    out (1), a

    ld (0x1000), a
    jp _hang


    .global _hang
    .type _hang, @function
_hang:
    halt
    jp _hang
