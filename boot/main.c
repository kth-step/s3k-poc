#include <stdbool.h>
#include <string.h>
#include "base.h"

#include "capman.h"
#include "payload.h"
#include "altio.h"
#include "s3k.h"
#include "../config.h"

#define BOOT_PID 0
#define MONITOR_PID 1
#define UART_PID    2
#define CRYPTO_PID  3
#define APP0_PID    4
#define APP1_PID    5

uint8_t pmpcaps[8] = {0};

void init_uart(void)
{
	pmpcaps[1] = capman_derive_pmp((uint64_t)UART_BASE, (uint64_t)UART_BASE + 0x8, S3K_RW);
	// Set pmp configuration
	capman_setpmp(pmpcaps);
}

void setup(void)
{
	// Initialize capman
	capman_init();
	init_uart();

	// We can now print stuff
	alt_puts("BOOT: Setting up.");

	alt_printf("testing: hello world\n"); 
	alt_printf("testing char: %c\n", 'c');
	alt_printf("testing string: %s\n", "hello world");
	alt_printf("testing hex: %x\n", 0xDEADBEEF);
	alt_printf("testing hex: %X\n", 0xdeaddeadbeefull);

	capman_derive_mem(MONITOR_BEGIN, MONITOR_END, S3K_RWX);
	capman_derive_mem(CRYPTO_BEGIN, CRYPTO_END, S3K_RWX);
	capman_derive_mem(UART_BEGIN, UART_END, S3K_RWX);
	capman_derive_mem(APP0_BEGIN, APP0_END, S3K_RWX);
	capman_derive_mem(APP1_BEGIN, APP1_END, S3K_RWX);
	alt_puts("BOOT: Setup complete.");
}

void loop(void)
{
	static int i = 0;
	char c[2] = "0";
	c[0] = '0' + (i % 10);
	i++;
	alt_puts(c);
	s3k_yield();
}
