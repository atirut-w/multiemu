#pragma once

#define IN(port) \
    ({ \
        unsigned char __port = (port); \
        unsigned char __value; \
        asm volatile ( \
            "ld c, %1\n\t" \
            "in a, (c)" \
            : "=a"(__value) \
            : "r"(__port) \
            : "c" \
        ); \
        __value; \
    })

#define OUT(port, value) \
    { \
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
    }
