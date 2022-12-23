#include "payload.h"
#include "printf.h"
#include <string.h>

#define BUFFER_COUNT 8
#define BUFFER_SIZE (1ull << 14)

char* buffers[BUFFER_SIZE];

void init_buffers(void)
{
        extern char _end[];
        for (int i = 0; i < BUFFER_COUNT; i++)
                buffers[i] = _end + i * BUFFER_SIZE;
}

void init_bin(void)
{
        memcpy(buffers[0], uart_bin, uart_bin_len);
        memcpy(buffers[1], app0_bin, app0_bin_len);
        memcpy(buffers[2], app1_bin, app1_bin_len);
}

typedef void (*fun_t)(void);

void main(void)
{
        init_buffers();
        init_bin();
        ((fun_t)buffers[0])();
}
