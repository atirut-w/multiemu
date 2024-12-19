    .org 0x0000
    .equ PAGETABLE, 0
    .global _start

    
    .global init
init:
    ld hl, 0x3000
    ld sp, hl
    ; Set up ROM mapping (2 pages)
    ld bc, 0
    xor a
    out (c), a

    inc bc
    inc a
    out (c), a

    ; Initialize memory
    ld bc, PAGETABLE + 2    ; Virtual-to-physical remap registers
    ld hl, 0x2000           ; Virtual page address
    ld d, 2                 ; First physical page to test
    ld e, 2                 ; Number of contiguous pages we want
0:
    out (c), d
    ld (hl), 0x55
    ld a, (hl)
    cp 0x55
    jr z, 1f

    inc d
    jr nz, 0b
    jr _hang
1:
    dec e
    jr z, 2f
    inc d
    inc bc
    push de
    ld de, 0x1000
    add hl, de
    pop de
    jr 0b
2:
    ld hl, 0x4000
    ld sp, hl
    call _start
    jr _hang

_hang:
    halt
    jr _hang
