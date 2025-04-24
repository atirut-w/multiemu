    .global __ishl
__ishl:
    bit 0, c
    jr z, 0f
    add hl, hl
0:
    bit 1, c
    jr z, 1f
    add hl, hl
    add hl, hl
1:
    bit 2, c
    jr z, 2f
    add hl, hl
    add hl, hl
    add hl, hl
    add hl, hl
    add hl, hl
2:
    bit 3, c
    jr nz, 4f
    bit 4, c
    ret z
3:
    ld h, l
    ld l, 0
4:
    add	hl, hl
    add	hl, hl
    add	hl, hl
    add	hl, hl
    add	hl, hl
    add	hl, hl
    add	hl, hl
    add	hl, hl
    ret
