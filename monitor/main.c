#include "altio.h"
#include "s3k.h"
#include "memory.h"

#include <stdbool.h>
#include <string.h>

#define MAIN_MEMORY_SLICE 0
#define MAIN_MEMORY_PMP 1
#define UART_PMP 2
#define APP0_MEMORY_SLICE 3
#define APP1_MEMORY_SLICE 4
#define SHARED_MEMORY_SLICE 4
#define UARTPPP_SERVER 6
#define ENCRYPT_CLIENT 7
#define DECRYPT_CLIENT 8
#define APPS_MONITOR 9
#define TIME_SLICE 10

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

struct memory *memory = (struct memory*)0x80000000ull;
uint64_t buf[4]  = {0,0,0,0};
uint64_t app_pid[2] = {4, 5};
uint64_t app_base[2];
uint64_t app_size[2];

void setup(void)
{
	alt_puts("monitor setup\n");
	// Yield to let crypto service intialize before revoking time.
	s3k_yield();
	// Revoke time beloning to crypto service.
	s3k_revcap(TIME_SLICE);

	// Initialize Apps
	app_base[0] = (uint64_t)memory->app0;
	app_base[1] = (uint64_t)memory->app1;
	app_size[0] = ARRAY_SIZE(memory->app0);
	app_size[1] = ARRAY_SIZE(memory->app1);
	uint64_t app0_addr = s3k_napot_encode(app_base[0], app_size[0]);
	uint64_t app1_addr = s3k_napot_encode(app_base[1], app_size[1]);

	// Get access to App memory
	s3k_drvcap(APP0_MEMORY_SLICE, 11, s3k_mkpmp(app0_addr, S3K_RW));
	s3k_drvcap(APP1_MEMORY_SLICE, 12, s3k_mkpmp(app1_addr, S3K_RW));
	s3k_pmpset(11, 2);
	s3k_pmpset(12, 3);

	// Give memory to applications
	s3k_drvcap(APP0_MEMORY_SLICE, 13, s3k_mkpmp(app0_addr, S3K_RWX));
	s3k_drvcap(APP1_MEMORY_SLICE, 14, s3k_mkpmp(app1_addr, S3K_RWX));
	s3k_mgivecap(APPS_MONITOR, app_pid[0], 13, 0);
	s3k_mgivecap(APPS_MONITOR, app_pid[1], 14, 0);
	s3k_mpmpset(APPS_MONITOR, app_pid[0], 0, 0);
	s3k_mpmpset(APPS_MONITOR, app_pid[1], 0, 0);
}

void command_exec(uint64_t app, uint8_t buf[8])
{
	uint64_t *pc = (uint64_t*)buf;
	uint64_t pid = app_pid[app];

	// If *pc != 0, then set new pc
	if (*pc)
		s3k_msetreg(APPS_MONITOR, pid, S3K_REG_PC, *pc);

	// Give 14 units of time
	s3k_cap_t time = s3k_mktime(0, 8, 14);
	s3k_drvcap(TIME_SLICE, 16, time);
	s3k_mgivecap(APPS_MONITOR, pid, 16, 16);

	// Yield, then revoke time.
	s3k_yield();
	s3k_revcap(TIME_SLICE);
}

void command_write(uint64_t app, uint8_t buf[16])
{
	uint8_t *base = (uint8_t*)app_base[app];
	uint64_t size = app_size[app];
	uint64_t len = buf[1] % 12;
	uint64_t offset = buf[3] << 8 | buf[2];

	for (int i = offset; i < offset + len && i < size; ++i) {
		base[i] = buf[i + 12];
	}
}

void command_read(uint64_t app, uint8_t buf[16])
{
	uint8_t *base = (uint8_t*)app_base[app];
	uint64_t size = app_size[app];
	uint64_t len = buf[1] % 12;
	uint64_t offset = buf[3] << 8 | buf[2];

	for (int i = offset; i < offset + len && i < size; ++i) {
		buf[i + 12] = base[i];
	}
}

void eval_command(uint8_t buf[16])
{
	uint64_t app = buf[0] & 1;
	switch (buf[0] >> 1) {
		case 0:
			command_exec(app, buf);
			break;
		case 1:
			command_write(app, buf);
			break;
		case 2:
			command_read(app, buf);
			break;
	}
}

void loop(void)
{
	uint64_t tag;
	s3k_cap_t cap;
	uint64_t buf_cidx = TIME_SLICE;

	s3k_sendrecv(UARTPPP_SERVER, buf, &tag);

	s3k_sendrecv_cap(DECRYPT_CLIENT, buf, buf_cidx, &cap, &tag);

	if (buf[2] == 0 && buf[3] == 0) { // checksum check.
		eval_command((uint8_t*)buf);
		s3k_sendrecv_cap(ENCRYPT_CLIENT, buf, buf_cidx, &cap, &tag);
	} else {
		buf[0] = buf[1] = buf[2] = buf[3] = 0;
	}

}
