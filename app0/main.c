#include <stdint.h>

#include "s3k.h"
#include "uart.h"

#define BUF_LENGTH 16

struct message {
        uint8_t buffer[BUF_LENGTH];
};

struct message msg;

void setup(void)
{
        uart_puts("app0 setup\n");
}

void loop(void)
{
        uart_puts("app0 loop\n");
        s3k_yield();
}
