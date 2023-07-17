/**
 * Splits up resources between processes.
 * Monitor:
 * - 0: PMP Frame for monitor executable (RWX)
 * - 1: Memory Slice for shared memory (RW)
 * - 2: Memory Slice for Application 0
 * - 3: Memory Slice for Application 1
 * - 4: Time slice on first hart
 * - 5: Server socket for communication with crypto
 * - 6: Server socket for communication with uartppp
 * Crypto:
 * - 0: 
 */

#include "../config.h"
#include "altio.h"
#include "memory.h"
#include "payload.h"
#include "ring_buffer.h"
#include "s3k.h"

#include <stdbool.h>
#include <string.h>

#define MEM_CIDX 1
#define UART_CIDX 2
#define TIME0_CIDX 4
#define TIME1_CIDX 5
#define TIME2_CIDX 6
#define TIME3_CIDX 7
#define MONCAP_CIDX 8
#define CHANCAP_CIDX 9

#define BOOT_PID 0
#define MONITOR_PID 1
#define CRYPTO_PID 2
#define UARTPPP_PID 3
#define APP0_PID 4
#define APP1_PID 5

struct memory *memory = (struct memory*)0x80000000;

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

void setup_memory(void)
{
	// Base and size of segments.
	uint64_t monitor_base = (uint64_t)memory->monitor;
	uint64_t monitor_size = ARRAY_SIZE(memory->monitor);
	uint64_t crypto_base = (uint64_t)memory->crypto;
	uint64_t crypto_size = ARRAY_SIZE(memory->crypto);
	uint64_t uartppp_base = (uint64_t)memory->uartppp;
	uint64_t uartppp_size = ARRAY_SIZE(memory->uartppp);
	uint64_t app0_base = (uint64_t)memory->app0;
	uint64_t app0_size = ARRAY_SIZE(memory->app0);
	uint64_t app1_base = (uint64_t)memory->app1;
	uint64_t app1_size = ARRAY_SIZE(memory->app1);
	uint64_t shared_base = (uint64_t)memory->shared;
	uint64_t shared_size = ARRAY_SIZE(memory->shared);

	// Base and size of segments.
	s3k_cap_t monitor_mem = s3k_mkmemory(monitor_base >> 12, monitor_size >> 12, S3K_RWX);
	s3k_cap_t crypto_mem = s3k_mkmemory(crypto_base >> 12, crypto_size >> 12, S3K_RWX);
	s3k_cap_t uartppp_mem = s3k_mkmemory(uartppp_base >> 12, uartppp_size >> 12, S3K_RWX);
	s3k_cap_t app0_mem = s3k_mkmemory(app0_base >> 12, app0_size >> 12, S3K_RWX);
	s3k_cap_t app1_mem = s3k_mkmemory(app1_base >> 12, app1_size >> 12, S3K_RWX);
	s3k_cap_t shared_mem = s3k_mkmemory(shared_base >> 12, shared_size >> 12, S3K_RW);

	// PMP Frames for monitor, crypto, uartppp, and uart
	uint64_t monitor_addr = s3k_napot_encode(monitor_base, monitor_size);
	uint64_t crypto_addr = s3k_napot_encode(crypto_base, crypto_size);
	uint64_t uartppp_addr = s3k_napot_encode(uartppp_base, uartppp_size);
	uint64_t uart_addr = 0x40001ff;
	s3k_cap_t monitor_pmp = s3k_mkpmp(monitor_addr, S3K_RWX);
	s3k_cap_t crypto_pmp = s3k_mkpmp(crypto_addr, S3K_RWX);
	s3k_cap_t uartppp_pmp = s3k_mkpmp(uartppp_addr, S3K_RWX);
	s3k_cap_t uart_pmp = s3k_mkpmp(uart_addr, S3K_RW);

	// Setup monitor memory
	s3k_drvcap(MEM_CIDX, 16, monitor_mem);
	s3k_drvcap(16, 17, monitor_pmp);
	s3k_drvcap(UART_CIDX, 18, uart_pmp);
	s3k_pmpset(17, 2);
	memcpy((void*)monitor_base, monitor_bin, monitor_bin_len);
	s3k_mgivecap(MONCAP_CIDX, MONITOR_PID, 16, 0);
	s3k_mgivecap(MONCAP_CIDX, MONITOR_PID, 17, 1);
	s3k_mgivecap(MONCAP_CIDX, MONITOR_PID, 18, 2);
	// Set monitor PC
	s3k_msetreg(MONCAP_CIDX, MONITOR_PID, S3K_REG_PC, monitor_base);
	// Set monitor PMP
	s3k_mpmpset(MONCAP_CIDX, MONITOR_PID, 1, 0);
	s3k_mpmpset(MONCAP_CIDX, MONITOR_PID, 2, 1);

	// Setup crypto memory
	s3k_drvcap(MEM_CIDX, 16, crypto_mem);
	s3k_drvcap(16, 17, crypto_pmp);
	s3k_drvcap(UART_CIDX, 18, uart_pmp);
	s3k_pmpset(17, 2);
	memcpy((void*)crypto_base, crypto_bin, crypto_bin_len);
	s3k_mgivecap(MONCAP_CIDX, CRYPTO_PID, 16, 0);
	s3k_mgivecap(MONCAP_CIDX, CRYPTO_PID, 17, 1);
	s3k_mgivecap(MONCAP_CIDX, CRYPTO_PID, 18, 2);
	// Set monitor PC
	s3k_msetreg(MONCAP_CIDX, CRYPTO_PID, S3K_REG_PC, crypto_base);
	// Set monitor PMP
	s3k_mpmpset(MONCAP_CIDX, CRYPTO_PID, 1, 0);
	s3k_mpmpset(MONCAP_CIDX, CRYPTO_PID, 2, 1);

	// Setup uartppp memory
	s3k_drvcap(MEM_CIDX, 16, uartppp_mem);
	s3k_drvcap(16, 17, uartppp_pmp);
	s3k_drvcap(UART_CIDX, 18, uart_pmp);
	s3k_pmpset(17, 2);
	memcpy((void*)uartppp_base, uartppp_bin, uartppp_bin_len);
	s3k_mgivecap(MONCAP_CIDX, UARTPPP_PID, 16, 0);
	s3k_mgivecap(MONCAP_CIDX, UARTPPP_PID, 17, 1);
	s3k_mgivecap(MONCAP_CIDX, UARTPPP_PID, 18, 2);
	// Set monitor PC
	s3k_msetreg(MONCAP_CIDX, UARTPPP_PID, S3K_REG_PC, uartppp_base);
	// Set monitor PMP
	s3k_mpmpset(MONCAP_CIDX, UARTPPP_PID, 1, 0);
	s3k_mpmpset(MONCAP_CIDX, UARTPPP_PID, 2, 1);

	// Give monitor app0, app1, and shared memory.
	s3k_drvcap(MEM_CIDX, 16, app0_mem);
	s3k_drvcap(MEM_CIDX, 17, app1_mem);
	s3k_drvcap(MEM_CIDX, 18, shared_mem);
	s3k_mgivecap(MONCAP_CIDX, MONITOR_PID, 16, 3);
	s3k_mgivecap(MONCAP_CIDX, MONITOR_PID, 17, 4);
	s3k_mgivecap(MONCAP_CIDX, MONITOR_PID, 18, 5);
}

void setup_ipc(void)
{
	/// Socket definitions.
	// MONITOR <-> UART
	// CRYPTO <-> MONITOR
	s3k_cap_t channel0_server = s3k_mksocket(0, 0);
	s3k_cap_t channel1_server = s3k_mksocket(1, 0);
	s3k_cap_t channel0_client = s3k_mksocket(0, 1);
	s3k_cap_t channel1_client1 = s3k_mksocket(1, 1);
	s3k_cap_t channel1_client2 = s3k_mksocket(1, 2);

	// Derive sockets
	s3k_drvcap(CHANCAP_CIDX, 16, channel0_server);
	s3k_drvcap(16, 17, channel0_client);
	s3k_drvcap(CHANCAP_CIDX, 18, channel1_server);
	s3k_drvcap(18, 19, channel1_client1);
	s3k_drvcap(18, 20, channel1_client2);

	// Hand out sockets.
	s3k_mgivecap(MONCAP_CIDX, MONITOR_PID, 16, 6);
	s3k_mgivecap(MONCAP_CIDX, UARTPPP_PID, 17, 3);
	s3k_mgivecap(MONCAP_CIDX, CRYPTO_PID, 18, 3);
	s3k_mgivecap(MONCAP_CIDX, MONITOR_PID, 19, 7);
	s3k_mgivecap(MONCAP_CIDX, MONITOR_PID, 20, 8);
}

void setup_monitoring(void)
{
	// Give monitor monitoring capability over APP0 and APP1
	s3k_cap_t tmp = s3k_mkmonitor(0, APP0_PID);
	s3k_cap_t monitor_app0_app1 = s3k_mkmonitor(APP0_PID, 2);
	
	s3k_drvcap(MONCAP_CIDX, 16, tmp);
	s3k_drvcap(MONCAP_CIDX, 17, monitor_app0_app1);
	s3k_delcap(MONCAP_CIDX);
	s3k_movcap(16, MONCAP_CIDX);
	s3k_mgivecap(MONCAP_CIDX, MONITOR_PID, 17, 9);
}

void setup_time(void)
{
	s3k_mresume(MONCAP_CIDX, MONITOR_PID);
	s3k_mresume(MONCAP_CIDX, CRYPTO_PID);
	s3k_mresume(MONCAP_CIDX, UARTPPP_PID);
	
	s3k_cap_t uartppp_time = s3k_mktime(0, 0, 4);
	s3k_cap_t monitor_time = s3k_mktime(0, 4, 28);
	s3k_cap_t crypto_time = s3k_mktime(0, 4, 14);

	s3k_drvcap(TIME0_CIDX, 16, uartppp_time);
	s3k_drvcap(TIME0_CIDX, 17, monitor_time);
	s3k_drvcap(17, 18, crypto_time);
	s3k_delcap(TIME0_CIDX);

	s3k_mgivecap(MONCAP_CIDX, UARTPPP_PID, 16, 4);
	s3k_mgivecap(MONCAP_CIDX, MONITOR_PID, 17, 10);
	s3k_mgivecap(MONCAP_CIDX, CRYPTO_PID, 18, 4);
}

void setup(void)
{
	// Delete time cap for harts 1,2,3.
	s3k_delcap(TIME1_CIDX);
	s3k_delcap(TIME2_CIDX);
	s3k_delcap(TIME3_CIDX);

	// Setup UART
	s3k_cap_t uart_pmp = s3k_mkpmp(0x40001ff, S3K_RW);
	s3k_drvcap(UART_CIDX, 15, uart_pmp);
	s3k_pmpset(15, 1);

	alt_puts("entry boot setup");
	setup_memory();
	setup_ipc();
	setup_monitoring();
	setup_time();
	alt_puts("exit boot setup");
}

void loop(void)
{
	while(1);
}
