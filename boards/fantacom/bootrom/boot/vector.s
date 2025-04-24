    .section .vectors
    .extern _init

rst00:
    jp _init

    .balign 8
rst08:
    ret
    .balign 8
rst10:
    ret
    .balign 8
rst18:
    ret
    .balign 8
rst20:
    ret
    .balign 8
rst28:
    ret
    .balign 8
rst30:
    ret
    .balign 8
rst38:
    ret
