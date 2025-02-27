#include <string.h>

extern char __data_start[], __data_end[], __data_load[];
extern char __bss_start[], __bss_end[];

void __libc_start_main(int (*main)(int, char **, char **), int argc, char **argv, void (*init)(void), void (*fini)(void), void (*rtld_fini)(void), void *stack_end);
int main(int argc, char **argv, char **env);

void start(void) {
  memcpy(__data_start, __data_load, __data_end - __data_start);
  memset(__bss_start, 0, __bss_end - __bss_start);

  __libc_start_main(main, 0, 0, 0, 0, 0, 0);
}

void _exit(int status) {
  while (1);
}
