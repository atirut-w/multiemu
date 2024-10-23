typedef struct DriverInfo {
  void (*init)(void);
} DriverInfo;

extern char data_start;
extern char data_end;
extern char data_load;

extern char bss_start;
extern char bss_end;

extern DriverInfo drivers_start;
extern DriverInfo drivers_end;

void init_data() {
  char *src = &data_load;
  char *dst = &data_start;
  while (dst < &data_end) {
    *dst++ = *src++;
  }
}

void init_bss() {
  char *dst = &bss_start;
  while (dst < &bss_end) {
    *dst++ = 0;
  }
}

void init_drivers() {
  DriverInfo *driver = &drivers_start;
  while (driver < &drivers_end) {
    driver->init();
    driver++;
  }
}
