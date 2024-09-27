    .section .text._start
    .global _start
    .type _start, @function
_start:
    jp _hang


    .section .text
    .global _hang
    .type _hang, @function
_hang:
    halt
    jp _hang
