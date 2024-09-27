    .global _start
    .type _start, @function
_start:
    ld a, 4
    out (1), a ; Map RAM into page 1

    ld (0x4000), a
    ret


    .global _hang
    .type _hang, @function
_hang:
    halt
    jp _hang
