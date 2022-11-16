#include "s3k.h"

static void uart_puts(const char *c) {
    while (*c != '\0') {
        uart_putchar(*c++);
    }
}

void main(void) {
    uart_puts("hello, init\n");
}
