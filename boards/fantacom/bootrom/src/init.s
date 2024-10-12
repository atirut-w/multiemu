    .section .startup
    .equ PAGETABLE, 0
    .equ START_PAGE, 2
    .equ VIRT_PAGE, 4
    
    .global _start
_start:
    ld bc, PAGETABLE + VIRT_PAGE ; Virtual-to-physical remap registers
    ld hl, VIRT_PAGE << 12 ; Virtual page address
    ld d, START_PAGE ; First physical page to test
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
    ld hl, VIRT_PAGE + 0x1000
    ld sp, hl
    jp _crt0_start
    jr _hang

_hang:
    halt
    jr _hang
