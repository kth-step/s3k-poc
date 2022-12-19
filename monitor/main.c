#include "payload.h"
#include "printf.h"
#include "util.h"

#define SLOTS 8

char* alloc[SLOTS];

void init_alloc(void)
{
        extern char _end[];
        for (int i = 0; i < SLOTS; i++)
                alloc[i] = _end + i * 16384;
}
void init_bin(void)
{
        memcpy(alloc[0], uart_bin_begin, (int)(uart_bin_end - uart_bin_begin));
        memcpy(alloc[1], app0_bin_begin, (int)(app0_bin_end - app0_bin_begin));
        memcpy(alloc[2], app1_bin_begin, (int)(app1_bin_end - app1_bin_begin));
}

typedef void (*void_fun_t)(void);

void main(void)
{
        init_alloc();
        init_bin();
        ((void_fun_t)alloc[0])();
}
