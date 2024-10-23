#include "io.h"
#include <stdint.h>

extern char font;

int main() {
  outl(16, (uint32_t)&font & 0xffff);
  return 0;
}
