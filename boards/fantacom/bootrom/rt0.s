    .global _start
    .type _start, @function
_start:
    ld bc, 16
    ld hl, charset

    out (c), l
    inc c
    out (c), h
    inc c
    xor a
    out (c), a
    jp _hang


    .global _hang
    .type _hang, @function
_hang:
    halt
    jp _hang
