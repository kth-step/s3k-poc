#include <stdbool.h>
#include <string.h>
#include "base.h"

#include "capman.h"
#include "payload.h"
#include "uart.h"
#include "ring_buffer.h"
#include "s3k.h"
#include "../config.h"

#define BOOT_PID 0
#define MONITOR_PID 1
#define UART_PID    2
#define CRYPTO_PID  3
#define APP0_PID    4
#define APP1_PID    5

uint8_t pmpcaps[8];

void setup(void)
{
	capman_init();
	capman_getpmp(pmpcaps);
	pmpcaps[1] = capman_derive_pmp((uint64_t)UART, (uint64_t)UART + 0x1000, S3K_RW);
	capman_setpmp(pmpcaps);
	uart_puts("BOOT: Setting up.");

	capman_derive_mem(MONITOR_BEGIN, MONITOR_END, S3K_RWX);
	capman_derive_mem(CRYPTO_BEGIN, CRYPTO_END, S3K_RWX);
	capman_derive_mem(UART_BEGIN, UART_END, S3K_RWX);
	capman_derive_mem(APP0_BEGIN, APP0_END, S3K_RWX);
	capman_derive_mem(APP1_BEGIN, APP1_END, S3K_RWX);
	uart_puts("BOOT: Setup complete.");
}

void loop(void)
{
	uart_putchar('.');
	s3k_yield();
}
