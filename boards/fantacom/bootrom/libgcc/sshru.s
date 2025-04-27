    .global __sshru
__sshru:
    inc c
    dec c
    ret z
    push bc
    ld b, c
0:
    srl h
    rr l
    djnz 0b
    pop bc
    ret
