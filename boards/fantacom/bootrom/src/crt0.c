#include <string.h>

extern char __data_start[], __data_end[], __data_load[];
extern char __bss_start[], __bss_end[];
extern void (*__init_array_start[])(void), (*__init_array_end[])(void);

void __libc_start_main(int (*main)(int, char **, char **), int argc, char **argv, void (*init)(void), void (*fini)(void), void (*rtld_fini)(void), void *stack_end);
int main(int argc, char **argv, char **env);

void start(void) {
  void (**p)(void);

  memcpy(__data_start, __data_load, __data_end - __data_start);
  memset(__bss_start, 0, __bss_end - __bss_start);

  for (p = __init_array_start; p < __init_array_end; p++) {
    (*p)();
  }

  __libc_start_main(main, 0, 0, 0, 0, 0, 0);
}

void _exit(int status) {
  while (1);
}
