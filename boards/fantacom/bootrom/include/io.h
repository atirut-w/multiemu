#define OUT(port, value) \
    do { \
        unsigned char __port = (port); \
        unsigned char __value = (value); \
        asm volatile ( \
            "ld a, %0\n\t" \
            "ld c, %1\n\t" \
            "out (c), a" \
            : \
            : "r"(__value), "r"(__port) \
            : "a", "c" \
        ); \
    } while (0)
