#include <string.h>

#include "payload.h"
#include "printf.h"

#define NBUFFER	    8
#define BUFFER_SIZE 16384

char buffers[NBUFFER][BUFFER_SIZE] __attribute__((section(".noinit")))
__attribute__((aligned(BUFFER_SIZE)));

void init_bin(void)
{
	memcpy(buffers[0], uart_bin, uart_bin_len);
	memcpy(buffers[1], app0_bin, app0_bin_len);
	memcpy(buffers[2], app1_bin, app1_bin_len);
}

typedef void (*fun_t)(void);

void main(void)
{
	init_bin();
	((fun_t)buffers[0])();
}
