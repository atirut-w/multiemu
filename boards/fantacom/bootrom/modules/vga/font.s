    .global _font
_font:
    .incbin "seabios8x16.bin"
    .global _font_len
_font_len:
    .short . - _font
