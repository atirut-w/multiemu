    .equ PAGETABLE, 0
    .equ TEST_AREA, 0x2000
    .equ START_PAGE, 2
    
    .global _start
_start:
    ld bc, (PAGETABLE+2)
    ld hl, TEST_AREA
    ld d, START_PAGE
0:
    out (c), d
    ld (hl), 0x55
    ld a, (hl)
    cp 0x55
    jr z, 2f

    inc d
    jr nz, 0b
1:
    ; We are cooked
    halt
    jr 1b
2:
    ; jp _crt0_start
    ret
