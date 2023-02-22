#include <stdbool.h>
#include <string.h>

#include "payload.h"
#include "s3k.h"

#define NOINIT	      __attribute__((section(".noinit")))
#define ALIGNED(x)    __attribute__((aligned(x)))
#define ARRAY_SIZE(x) ((sizeof(x)) / (sizeof(x[0])))

#define MEMSIZE 4096

#define MONITOR_PID 0
#define UART_PID    1
#define CRYPTO_PID  3
#define APP0_PID    5
#define APP1_PID    6

#define MONCAP 6

/* Program memory */
char uart_mem[MEMSIZE] NOINIT ALIGNED(MEMSIZE);
char crypto_mem[MEMSIZE] NOINIT ALIGNED(MEMSIZE);
char app0_mem[2][MEMSIZE] NOINIT ALIGNED(MEMSIZE);
char app1_mem[2][MEMSIZE] NOINIT ALIGNED(MEMSIZE);

/* Shared memory */
volatile char uart_buf[64] NOINIT ALIGNED(64);

void setup_uart(void)
{
	s3k_msetreg(MONCAP, UART_PID, S3K_REG_PC, (uint64_t)uart_mem);
	s3k_msetreg(MONCAP, UART_PID, S3K_REG_A0, (uint64_t)uart_buf);
	s3k_msetreg(MONCAP, UART_PID, S3K_REG_A1, (uint64_t)sizeof(uart_buf));
	/* TODO: Memory/PMP slices */
}

void setup(void)
{
	/* Copy text and data to memory segments */
	memcpy(uart_mem, uart_bin, uart_bin_len);
	// memcpy(crypto_mem, crypto_bin, crypto_bin_len);
	memcpy(app0_mem[0], app0_bin, app0_bin_len);
	memcpy(app1_mem[0], app1_bin, app1_bin_len);

	setup_uart();

	/* Delete time on core 2 3 4. */
	s3k_delcap(3);
	s3k_delcap(4);
	s3k_delcap(5);

	/* Derive time for uart */
	s3k_drvcap(2, 10, s3k_time(0, 0, 4));
	s3k_mgivecap(MONCAP, UART_PID, 10, 2);

	/* Start uart */
	s3k_mresume(MONCAP, UART_PID);
	s3k_yield();
}

void loop(void)
{
	if (!uart_buf[32])
		s3k_yield();
	switch (uart_buf[0]) {
	/* App 0 cases */
	case 0x00: /* Set PC to memory 0 */
		s3k_msetreg(MONCAP, APP0_PID, S3K_REG_PC, (uint64_t)app0_mem[0]);
		break;
	case 0x01: /* Set PC to memory 1 */
		s3k_msetreg(MONCAP, APP0_PID, S3K_REG_PC, (uint64_t)app0_mem[1]);
		break;
	case 0x02: /* Write to memory 0 */
		break;
	case 0x03: /* Write to memory 1 */
		break;
	case 0x0F: /* Copy data from uart to app1 */
		break;
	/* App 1 cases */
	case 0x10: /* Set PC to memory 0 */
		s3k_msetreg(MONCAP, APP1_PID, S3K_REG_PC, (uint64_t)app1_mem[0]);
		break;
	case 0x11: /* Set PC to memory 1 */
		s3k_msetreg(MONCAP, APP1_PID, S3K_REG_PC, (uint64_t)app1_mem[1]);
		break;
	case 0x12: /* Write to memory 0 */
		break;
	case 0x13: /* Write to memory 1 */
		break;
	case 0x1F: /* Copy data from uart to app1 */
		break;
	case 0xFF:
		break;
	}
	s3k_yield();
}
