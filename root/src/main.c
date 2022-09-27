__asm__(".section .text.init");

static inline void uart_init(void)
{
    unsigned char* uart = (unsigned char*)0x10000000;
    uart[3] = 3;
    uart[2] = 1;
}

static inline int uart_putchar(char c)
{
    unsigned char* uart = (unsigned char*)0x10000000;
    uart[0] = c;
    return c;
}

static inline int uart_getchar(void)
{
    unsigned char* uart = (unsigned char*)0x10000000;
    if (uart[5] & 1)
        return uart[0];
    return 0;
}

static void puts(char *c) {
    while (*c != '\0')
        uart_putchar(*c);
}

int main(void) {
    puts("hello, world");
}
