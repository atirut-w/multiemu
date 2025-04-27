extern char __data_start[], __data_end[], __data_load[];
extern char __bss_start[], __bss_end[];
extern void (*__init_array_start[])(void), (*__init_array_end[])(void);

int main(int argc, char **argv, char **env);

void start(void) {
  void (**p)(void);
  void *src, *dst;

  src = __data_load;
  dst = __data_start;
  while (dst < __data_end) {
    *(char *)dst = *(char *)src;
    src = (char *)src + 1;
    dst = (char *)dst + 1;
  }

  src = __bss_start;
  dst = __bss_end;
  while (src < dst) {
    *(char *)src = 0;
    src = (char *)src + 1;
  }

  for (p = __init_array_start; p < __init_array_end; p++) {
    (*p)();
  }

  main(0, 0, 0);
  while (1);
}

void _exit(int status) {
  while (1);
}
