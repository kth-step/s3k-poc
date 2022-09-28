#define INIT __attribute__(".section .text.init");

static inline void uart_init(void)
{
    volatile unsigned char* uart = (volatile unsigned char*)0x10000000;
    uart[3] = 3;
    uart[2] = 1;
}

static inline int uart_putchar(char c)
{
    volatile unsigned char* uart = (volatile unsigned char*)0x10000000;
    uart[0] = c;
    return c;
}

static inline int uart_getchar(void)
{
    volatile unsigned char* uart = (volatile unsigned char*)0x10000000;
    if (uart[5] & 1)
        return uart[0];
    return 0;
}

static void uart_puts(const char *c) {
    for (;*c != '\0'; c++)
        uart_putchar(*c);
}


int main(void) {
    while (1)
        uart_puts("hello, world\n");
}
