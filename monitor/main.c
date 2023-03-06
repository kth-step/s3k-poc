#include <stdbool.h>
#include <string.h>

#include "payload.h"
#include "ring_buffer.h"
#include "s3k.h"

#define NOINIT	      __attribute__((section(".noinit")))
#define ALIGNED(x)    __attribute__((aligned(x)))
#define ARRAY_SIZE(x) ((sizeof(x)) / (sizeof(x[0])))

#define MEMSIZE 8192

#define MONITOR_PID 0
#define UART_PID    1
#define CRYPTO_PID  3
#define APP0_PID    5
#define APP1_PID    6

#define CAP_PMP_INIT 0
#define CAP_MEM_MAIN 1
#define CAP_MEM_UART 2
#define CAP_TIME0    3
#define CAP_TIME1    4
#define CAP_TIME2    5
#define CAP_TIME3    6
#define CAP_MON	     7
#define CAP_CHAN     8

struct ring_buffer *uart_ring_buffer;

/* Program memory */
char uart_mem[MEMSIZE] NOINIT ALIGNED(MEMSIZE);
char crypto_mem[MEMSIZE] NOINIT ALIGNED(MEMSIZE);
char app0_mem[2][MEMSIZE] NOINIT ALIGNED(MEMSIZE);
char app1_mem[2][MEMSIZE] NOINIT ALIGNED(MEMSIZE);

/* Shared memory */
volatile char uart_buf[64] NOINIT ALIGNED(64);
volatile char app0_buf[32] NOINIT ALIGNED(32);
volatile char app1_buf[32] NOINIT ALIGNED(32);

void setup_uart(void)
{
	// Set registers
	s3k_msetreg(CAP_MON, UART_PID, S3K_REG_PC, (uint64_t)uart_mem);
	s3k_msetreg(CAP_MON, UART_PID, S3K_REG_A0, (uint64_t)uart_buf);
	s3k_msetreg(CAP_MON, UART_PID, S3K_REG_A1, (uint64_t)sizeof(uart_buf));
	// PMP in index 0,1,2
	s3k_msetreg(CAP_MON, UART_PID, S3K_REG_PMP, 0x020100);

	// Derive memory for uart
	uint64_t uartaddr = s3k_pmp_napot_addr(0x10000000, 0x10001000);
	uint64_t mainaddr = s3k_pmp_napot_addr((uint64_t)uart_mem, (uint64_t)uart_mem + MEMSIZE);
	uint64_t bufferaddr = s3k_pmp_napot_addr((uint64_t)uart_buf, (uint64_t)uart_buf + 64);
	union s3k_cap uartcap = s3k_pmp(uartaddr, S3K_RW);
	union s3k_cap maincap = s3k_pmp(mainaddr, S3K_RWX);
	union s3k_cap buffercap = s3k_pmp(bufferaddr, S3K_RW);
	while (s3k_drvcap(CAP_MEM_UART, 10, uartcap))
		;
	while (s3k_drvcap(CAP_MEM_MAIN, 11, maincap))
		;
	while (s3k_drvcap(CAP_MEM_MAIN, 12, buffercap))
		;
	s3k_setreg(S3K_REG_PMP, 0x0b00);
	// Copy the uart bin to uart's main memory.
	memcpy(uart_mem, uart_bin, uart_bin_len);

	while (s3k_mgivecap(CAP_MON, UART_PID, 11, 0))
		;
	while (s3k_mgivecap(CAP_MON, UART_PID, 12, 1))
		;
	while (s3k_mgivecap(CAP_MON, UART_PID, 10, 2))
		;

	/* Derive time for uart */
	while (s3k_drvcap(CAP_TIME0, 13, s3k_time(0, 0, 4)))
		;
	while (s3k_mgivecap(CAP_MON, UART_PID, 13, 3))
		;
}

// void setup_channels()
//{
//	union s3k_cap serv_socket = s3k_socket(0, 0);
//	union s3k_cap client0_socket = s3k_socket(0, 1);
//	union s3k_cap client1_socket = s3k_socket(0, 2);
//	s3k_drvcap(CAP_CHAN, 16, serv_socket);
//	s3k_drvcap(16, 17, client0_socket);
//	s3k_drvcap(16, 18, client1_socket);
//	s3k_mgivecap(CAP_MON, APP0_PID, 17, 16);
//	s3k_mgivecap(CAP_MON, APP1_PID, 18, 17);
//
//	// Server
//	uint64_t tag;
//	uint64_t msgs[4];
//	s3k_recv(16, &tag, msgs, dst_idx);
//	// Client
//	uint64_t msgs[4];
//	bool yield;
//	s3k_send(16, msg, src_idx, yield);
//	// Call
//	uint64_t tag;
//	uint64_t msgs[4];
//	s3k_sendrecv(16, 17, &tag, msg, src_idx, dst_idx);
// }

void setup_monitor(void)
{
	// Caps for monitor
	uint64_t bufferaddr = s3k_pmp_napot_addr((uint64_t)uart_buf, (uint64_t)uart_buf + 64);
	union s3k_cap buffercap = s3k_pmp(bufferaddr, S3K_RW);
	while (s3k_drvcap(CAP_MEM_MAIN, 10, buffercap))
		;
	s3k_setreg(S3K_REG_PMP, 0x0a00);
}

void setup(void)
{
	/* Delete time on core 2 3 4. */
	s3k_delcap(CAP_TIME1);
	s3k_delcap(CAP_TIME2);
	s3k_delcap(CAP_TIME3);
	/* Copy text and data to memory segments */
	// memcpy(crypto_mem, crypto_bin, crypto_bin_len);
	// memcpy(app0_mem[0], app0_bin, app0_bin_len);
	// memcpy(app1_mem[0], app1_bin, app1_bin_len);

	setup_uart();
	setup_monitor();

	/* Start uart */
	s3k_mresume(CAP_MON, UART_PID);
	s3k_yield();
}

void loop(void)
{
	switch (uart_buf[0]) {
	/* App 0 cases */
	case 0x01: /* Set PC to memory 0 */
		s3k_msetreg(CAP_MON, APP0_PID, S3K_REG_PC, (uint64_t)app0_mem[0]);
		break;
	case 0x02: /* Set PC to memory 1 */
		s3k_msetreg(CAP_MON, APP0_PID, S3K_REG_PC, (uint64_t)app0_mem[1]);
		break;
	case 0x03: /* Write to memory 0 */
		break;
	case 0x04: /* Write to memory 1 */
		break;
	case 0x05: /* Copy data from uart to app1 */
		break;
	/* App 1 cases */
	case 0x11: /* Set PC to memory 0 */
		s3k_msetreg(CAP_MON, APP1_PID, S3K_REG_PC, (uint64_t)app1_mem[0]);
		break;
	case 0x12: /* Set PC to memory 1 */
		s3k_msetreg(CAP_MON, APP1_PID, S3K_REG_PC, (uint64_t)app1_mem[1]);
		break;
	case 0x13: /* Write to memory 0 */
		break;
	case 0x14: /* Write to memory 1 */
		break;
	case 0x15: /* Copy data from uart to app1 */
		break;
	default:
		break;
	}
	s3k_yield();
}
