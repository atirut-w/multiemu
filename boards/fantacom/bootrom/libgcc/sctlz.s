    .global __sctlz
__sctlz:
    push hl
    scf
    sbc a, a
0:
    adc hl, hl
    inc a
    jr nc, 0b

    pop hl
    ret
