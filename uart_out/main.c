#include <stdint.h>

#include "s3k.h"
#include "uart.h"

#define BUF_LENGTH 14

struct message {
        uint8_t buffer[BUF_LENGTH];
};

struct message msg;

void setup(void)
{
        uart_puts("uart_out setup\n");
}

void loop(void)
{
        s3k_yield();
}
