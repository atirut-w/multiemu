    ; C runtime library for EZ80 Clang

    ; Shift right signed
    ; Parameters:
    ;  HL: value to shift
    ;  C: number of bits to shift
    ; Returns:
    ;  HL: shifted value
    .global __sshrs
    .type __sshrs, @function
__sshrs:
    xor a ; Clear carry
0:
    ld a, h
    rra
    ld h, a
    ld a, l
    rra
    ld l, a
    dec c
    jr nz, 0b
    ret
