#include "io.h"

extern unsigned char font[];
extern unsigned int font_len;

__attribute__((constructor)) void vga_init() {
  unsigned char *font_ptr = font;
  
  OUT(15, 64); // Map in VRAM at page 16

  for (unsigned i = 0; i < font_len; i++) {
    *(unsigned char *)(4096 * 15 + i) = font_ptr[i];
  }

  OUT(15, 65); // Map in VGA buffer

  const char *text = "Hello, World!";
  char *vga = (char *)(4096 * 15);
  for (unsigned i = 0; text[i] != '\0'; i++) {
    *vga++ = text[i];
    *vga++ = 0x07; // Attribute byte (light gray on black)
  }
}
