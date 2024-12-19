#include <string.h>

typedef struct DriverInfo {
  void (*init)(void);
} DriverInfo;

extern char data_start[];
extern char data_end[];
extern char data_load[];

extern char initializer_start[];
extern char initializer_end[];
extern char initialized_start[];

extern char bss_start[];
extern char bss_end[];

extern DriverInfo drivers_start[];
extern DriverInfo drivers_end[];

extern int main(int argc, char *argv[]);

void init_data() {
  char *src = data_load;
  char *dst = data_start;
  memcpy(dst, src, data_end - data_start);
  memcpy(initialized_start, initializer_start, initializer_end - initializer_start);
}

void init_bss() {
  char *dst = bss_start;
  while (dst < bss_end) {
    *dst++ = 0;
  }
}

void init_drivers() {
  DriverInfo *driver = drivers_start;
  while (driver < drivers_end) {
    driver->init();
    driver++;
  }
}

void start() {
  init_data();
  init_bss();
  init_drivers();
  main(0, 0);
}
