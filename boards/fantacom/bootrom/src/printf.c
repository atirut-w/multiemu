#include <stdarg.h>
#include <stdio.h>

int print_dec(int n) {
  int len = 0;

  if (n < 0) {
    putchar('-');
    len++;
    n = -n;
  }

  if (n / 10) {
    len += print_dec(n / 10);
  }

  putchar(n % 10 + '0');
  return len + 1;
}

int printf(const char *format, ...) {
  va_list args;
  va_start(args, format);
  int len = 0;

  while (*format) {
    if (*format == '%') {
      format++;

      switch (*format) {
      case '%':
        putchar('%');
        len++;
        break;
      case 'd':
        len += print_dec(va_arg(args, int));
        break;
      case 's':
        len += puts(va_arg(args, char *));
        break;
      }

      format++;
    } else {
      putchar(*format++);
      len++;
    }
  }

  va_end(args);
  return len;
}
