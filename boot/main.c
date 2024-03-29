#include <stdbool.h>
#include <string.h>

#include "../config.h"
#include "altio.h"
#include "base.h"
#include "capman.h"
#include "payload.h"
#include "ring_buffer.h"
#include "s3k.h"

#define BOOT_PID    0
#define MONITOR_PID 1
#define CRYPTO_PID  2
#define UARTPPP_PID 3
#define APP0_PID    4
#define APP1_PID    5

uint8_t pmpcaps[8] = {0};
char trapstack[4096];

void traphandler(void) __attribute__((interrupt("user")));

void traphandler(void)
{
	static int pmp = 4;
	//	uint64_t epc = s3k_getreg(S3K_REG_EPC);
	uint64_t ecause = s3k_getreg(S3K_REG_ECAUSE);
	uint64_t eval = s3k_getreg(S3K_REG_EVAL);
	if (ecause == 0x7) {
		// Try derive a pmp capability
		uint64_t size = 0x1 << 27;
		int i = capman_find_free();
		while (size >= 0x1000) {
			uint64_t begin = eval & ~(size - 1);
			uint64_t end = begin + size;
			if (capman_derive_pmp(i, begin, end, S3K_RW)) {
				// If a pmp cap was found use it
				if (pmpcaps[pmp])
					capman_delcap(pmpcaps[pmp]);
				pmpcaps[pmp] = i;
				capman_setpmp(pmpcaps);
				pmp++;
				if (pmp == 8)
					pmp = 4;
				return;
			}
			size >>= 1;
		}
	}
}

void setup_memory_slices(void)
{
	// Monitor slice
	capman_derive_mem(0x10, MONITOR_BASE, MONITOR_BASE + 0x4000, S3K_RWX);
	// Crypto slice
	capman_derive_mem(0x11, CRYPTO_BASE, CRYPTO_BASE + 0x4000, S3K_RWX);
	// Uart PPP slice
	capman_derive_mem(0x12, UARTPPP_BASE, UARTPPP_BASE + 0x4000, S3K_RWX);
	// Application slice
	capman_derive_mem(0x13, APP0_BASE, SHARED_BASE, S3K_RWX);
	// Shared memory slice
	capman_derive_mem(0x14, SHARED_BASE, SHARED_BASE + 0x10000, S3K_RW);
	// Delete remainder
	capman_delcap(0x1);
}

void setup_time_slices(void)
{
	uint64_t hartid = 0;
	// UART
	capman_derive_time(0x18, hartid, 0, 8);
	// CRYPTO
	capman_derive_time(0x19, hartid, 8, 16);
	// MONITOR
	capman_derive_time(0x1a, hartid, 16, 48);
	// CRYPTO
	capman_derive_time(0x1b, hartid, 48, 56);
	// UART
	capman_derive_time(0x1c, hartid, 56, 64);
	// Temporary time slice for bootloader
	capman_derive_time(0x1d, hartid, 16, 32);
}

void setup_monitor(void)
{
	// Copy over monitor binary
	capman_derive_pmp(0x20, MONITOR_BASE, MONITOR_BASE + 0x4000, S3K_RWX);
	pmpcaps[1] = 0x20;
	capman_setpmp(pmpcaps);
	memcpy((void *)MONITOR_BASE, monitor_bin, monitor_bin_len);

	// *** Give resources
	capman_msetreg(MONITOR_PID, S3K_REG_PC, MONITOR_BASE);
	// Give monitor PMP slice
	capman_mgivecap(MONITOR_PID, 0x20, 0x0);
	// Give monitor memory slice
	capman_mgivecap(MONITOR_PID, 0x10, 0x10);
	// Give application memory slice
	capman_mgivecap(MONITOR_PID, 0x13, 0x11);
	// Give monitor time slice
	capman_mgivecap(MONITOR_PID, 0x1a, 0x18);

	// UART access
	capman_derive_pmp(0x20, (uint64_t)UART_BASE, (uint64_t)UART_BASE + 0x8, S3K_RW);
	capman_mgivecap(MONITOR_PID, 0x20, 0x1);

	// Set PC
	capman_msetreg(MONITOR_PID, S3K_REG_PC, MONITOR_BASE);
	capman_msetreg(MONITOR_PID, S3K_REG_PMP, 0x0100);
}

void setup_crypto(void)
{
	// Copy over crypto binary
	capman_derive_pmp(0x20, CRYPTO_BASE, CRYPTO_BASE + 0x4000, S3K_RWX);
	pmpcaps[1] = 0x20;
	capman_setpmp(pmpcaps);
	memcpy((void *)CRYPTO_BASE, crypto_bin, crypto_bin_len);

	// *** Give resources
	capman_msetreg(CRYPTO_PID, S3K_REG_PC, CRYPTO_BASE);
	// Give crypto PMP slice
	capman_mgivecap(CRYPTO_PID, 0x20, 0x0);
	// Give crypto memory slice
	capman_mgivecap(CRYPTO_PID, 0x11, 0x10);
	// Give crypto time slices
	capman_mgivecap(CRYPTO_PID, 0x19, 0x19);
	capman_mgivecap(CRYPTO_PID, 0x1b, 0x1b);

	// UART access
	capman_derive_pmp(0x20, (uint64_t)UART_BASE, (uint64_t)UART_BASE + 0x8, S3K_RW);
	capman_mgivecap(CRYPTO_PID, 0x20, 0x1);

	// Set PC
	capman_msetreg(CRYPTO_PID, S3K_REG_PC, CRYPTO_BASE);
	capman_msetreg(CRYPTO_PID, S3K_REG_PMP, 0x0100);
}

void setup_uartppp(void)
{
	// Copy over monitor binary
	capman_derive_pmp(0x20, UARTPPP_BASE, UARTPPP_BASE + 0x4000, S3K_RWX);
	pmpcaps[1] = 0x20;
	capman_setpmp(pmpcaps);
	memcpy((void *)UARTPPP_BASE, uartppp_bin, uartppp_bin_len);

	// *** Give resources
	capman_msetreg(UARTPPP_PID, S3K_REG_PC, UARTPPP_BASE);
	// Give uart PMP slice
	capman_mgivecap(UARTPPP_PID, 0x20, 0x0);
	capman_derive_pmp(0x20, (uint64_t)UART_BASE, (uint64_t)UART_BASE + 0x8, S3K_RW);
	capman_mgivecap(UARTPPP_PID, 0x20, 0x1);
	// Give uart memory slice
	capman_mgivecap(UARTPPP_PID, 0x12, 0x10);
	// Give uart time slices
	capman_mgivecap(UARTPPP_PID, 0x18, 0x19);
	capman_mgivecap(UARTPPP_PID, 0x1c, 0x1b);

	// Set PC
	capman_msetreg(UARTPPP_PID, S3K_REG_PC, UARTPPP_BASE);
	capman_msetreg(UARTPPP_PID, S3K_REG_PMP, 0x0100);
}

void setup(void)
{
	s3k_delcap(4);
	s3k_delcap(5);
	s3k_delcap(6);

	// Setup trap handler
	s3k_setreg(S3K_REG_TPC, (uint64_t)traphandler);
	s3k_setreg(S3K_REG_TSP, (uint64_t)trapstack + sizeof(trapstack));

	// Initialize capman
	capman_init();

	// We can now print stuff
	alt_puts("\nboot: Setting up.");

	alt_puts("\ntesting alt_printf");
	alt_puts("------------------");
	alt_printf("plain: hello, world\n");
	alt_printf("char %%c: %c\n", 'A');
	alt_printf("string %%s: %s\n", "hello, world");
	alt_printf("4B hex %%x: 0x%x\n", 0xDEADBEEF);
	alt_printf("8B hex %%X: 0x%X\n", 0xcafedeadbeefull);
	alt_puts("------------------");

	setup_memory_slices();
	setup_time_slices();
	setup_monitor();
	setup_crypto();
	setup_uartppp();

	alt_puts("\nboot: Setup complete.");

	for (int i = 0; i < NPROC; ++i) {
		alt_printf("\nboot: proc 0x%x capabilities\n", i);
		alt_puts("------------------");
		for (int j = 0; j < NCAP; ++j) {
			union s3k_cap cap;
			if (i) {
				s3k_mgetcap(0x7, i, j, &cap);
			} else {
				cap = s3k_getcap(j);
			}
			if (cap.raw == 0)
				continue;
			alt_printf("0x%x: ", j);
			capman_dump(cap);
		}
		alt_puts("------------------");
	}
	s3k_yield();

	// Create capabilities for IPC
	s3k_drvcap(0x8, 0x9, s3k_socket(0, 0));
	s3k_drvcap(0x9, 0xa, s3k_socket(0, 1));
	s3k_mgivecap(0x7, CRYPTO_PID, 0x9, 0x9);
	s3k_mgivecap(0x7, MONITOR_PID, 0xa, 0xa);
	s3k_drvcap(0x8, 0x9, s3k_socket(1, 0));
	s3k_drvcap(0x9, 0xa, s3k_socket(1, 1));
	s3k_mgivecap(0x7, MONITOR_PID, 0x9, 0x9);
	s3k_mgivecap(0x7, CRYPTO_PID, 0xa, 0xa);

	capman_mresume(UARTPPP_PID);
	capman_mresume(CRYPTO_PID);
	capman_mresume(MONITOR_PID);
	s3k_yield();
}

void loop(void)
{
	alt_putstr(".");
	s3k_yield();
}
