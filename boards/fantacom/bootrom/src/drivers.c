typedef struct DriverInfo {
  void (*init)(void);
} DriverInfo;

extern DriverInfo drivers_start;
extern DriverInfo drivers_end;

void init_drivers() {
  DriverInfo *driver = &drivers_start;
  while (driver < &drivers_end) {
    driver->init();
    driver++;
  }
}
