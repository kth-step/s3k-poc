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
        uart_puts("uart_in setup\n");
}

bool receive_msg()
{
        for (int i = 0; i < BUF_LENGTH; i++) {
                msg.buffer[i] = uart_getchar();
        }

        uint64_t checksum = uart_getchar();

        for (int i = 0; i < BUF_LENGTH; i++)
                checksum ^= msg.buffer[i];

        return checksum == 0;
}

void loop(void)
{
        uart_puts("uart_in loop\n");
        s3k_yield();
}
