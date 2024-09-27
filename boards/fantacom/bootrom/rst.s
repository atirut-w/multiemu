    .section .text.rst

    .global _reset
    .type _reset, @function
_reset:
    jp _start
    jp _hang

.rept 7
    .balign 8
    jp _hang
.endr
