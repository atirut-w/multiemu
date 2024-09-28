    .global _start
    .type _start, @function
_start:
    ld bc, 16
    ld hl, charset

    out (c), l
    inc bc
    out (c), h
    inc bc
    xor a
    out (c), a

    ; ld hl, hello
    ; inc bc
    ; out (c), l
    ; inc bc
    ; out (c), h
    ; inc bc
    ; xor a
    ; out (c), a
    jp _hang


    .global _hang
    .type _hang, @function
_hang:
    halt
    jp _hang


    .section .data
hello:
    .db 'H', 0x07, 'e', 0x07, 'l', 0x07, 'l', 0x07, 'o', 0x07, '!', 0x07, 0x00
