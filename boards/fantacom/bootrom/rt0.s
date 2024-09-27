    .global _start
    .type _start, @function
_start:
    ret


    .global _hang
    .type _hang, @function
_hang:
    halt
    jp _hang
