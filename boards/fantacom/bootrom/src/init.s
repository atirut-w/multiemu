    .section .startup
    .equ PAGETABLE, 0

    
    .global _start
_start:
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
0:
    out (c), d
    ld (hl), 0x55
    ld a, (hl)
    cp 0x55
    jr z, 2f

    inc d
    jr nz, 0b
    jr _hang
2:
    ld hl, 0x3000
    ld sp, hl
    call init_drivers
    call main
    jr _hang

_hang:
    halt
    jr _hang

    .global __sdcc_call_hl
__sdcc_call_hl:
    jp (hl)
    ret
