#include "io.h"
#include "output.h"

typedef struct {
  unsigned char character;
  unsigned char attribute;
} vga_char_t;

extern unsigned char font[];
vga_char_t *vga = (vga_char_t *)(0xf000);
int cursor = 0;

static void decompress(unsigned char *dest, const unsigned char *src, unsigned int len) {
  unsigned int src_idx = 0;
  unsigned int dest_idx = 0;
  
  while (src_idx < len) {
    unsigned char control = src[src_idx++];
    
    if (control == 0) {
      // EOF marker
      break;
    } else if (control >= 128) {
      // Compressed sequence
      // (control - 126) gives sequence length between 2 and 129
      unsigned char repeat_count = control - 126;
      unsigned char value = src[src_idx++];
      
      for (unsigned char j = 0; j < repeat_count; j++) {
        dest[dest_idx++] = value;
      }
    } else {
      // Unique sequence
      // control gives sequence length between 1 and 127
      for (unsigned char j = 0; j < control; j++) {
        dest[dest_idx++] = src[src_idx++];
      }
    }
  }
}

__attribute__((constructor)) void vga_init() {
  unsigned char *font_ptr = font;
  
  OUT(15, 64); // Map in VRAM at page 16

  // for (unsigned i = 0; i < font_len; i++) {
  //   *(unsigned char *)(4096 * 15 + i) = font_ptr[i];
  // }
  decompress((unsigned char *)(4096 * 15), font_ptr, 4096);

  puts("Hello, world!");
}

void puts(const char *str) {
  int old_page = IN(15);
  OUT(15, 65); // Map in VRAM at page 16

  while (*str) {
    vga[cursor].character = *str;
    vga[cursor].attribute = 0x07; // White on black
    cursor++;
    str++;
  }
  vga[cursor].character = '\n';
  vga[cursor].attribute = 0x07; // White on black
  cursor++;

  OUT(15, old_page); // Restore old page
}
