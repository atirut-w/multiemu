_start:
    ld bc, 16
    ld hl, font

    out (c), l
    inc c
    out (c), h
    xor a
    out (c), a
    inc c
    out (c), a
    ret
