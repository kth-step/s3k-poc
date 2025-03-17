#include "s3k.h"
#include "serio.h"
#include "serio/uart/ns16550a.h"

SERIOFILE _uartfile = SERIO_UART_NS16550A(0x10000000);
SERIOFILE *const _serio_out = &_uartfile;

enum {
	BOOT_PMP = 0,
	RAM_MEM = 1,
	UART_MEM = 2,
	TIMER_MEM = 3,
	HART0_TIME = 4,
	MONITOR = 5,
	CHANNEL = 6,
};

enum {
	PID_BOOT = 0,
	PID_COMM = 1,
	PID_FCS = 2,
	PID_NAV = 3,
};

void error(int err)
{
	if (err) {
		serio_printf("Error %s\n", s3k_err2str(err));
		while (1) {
		}
	}
}

void setup_process(int pid, uint64_t addr)
{
	s3k_cap_t uart = s3k_mk_pmp(s3k_napot_encode(0x10000000, 0x20), 0x3);
	s3k_cap_t ram = s3k_mk_pmp(s3k_napot_encode(addr, 0x10000), 0x7);
	// Derive RAM
	error(s3k_cap_derive(RAM_MEM, 10, ram));
	error(s3k_mon_cap_send(MONITOR, 10, pid, 0));
	error(s3k_mon_pmp_load(MONITOR, pid, 0, 0));
	// Derive UART
	error(s3k_cap_derive(UART_MEM, 10, uart));
	error(s3k_mon_cap_send(MONITOR, 10, pid, 1));
	error(s3k_mon_pmp_load(MONITOR, pid, 1, 1));
	// Derive communication channel
	s3k_chan_t channel = pid;
	s3k_ipc_mode_t mode = S3K_IPC_YIELD;
	s3k_ipc_perm_t perm = S3K_IPC_CCAP;
	error(s3k_cap_derive(CHANNEL, 9, s3k_mk_socket(channel, mode, perm, 0)));
	error(s3k_cap_derive(9, 10 + pid, s3k_mk_socket(channel, mode, perm, 1)));
	error(s3k_mon_cap_send(MONITOR, 9, pid, 2));

	// Set PC
	error(s3k_mon_reg_write(MONITOR, pid, S3K_REG_PC, addr));

	// Start process
	error(s3k_mon_resume(MONITOR, pid));
}

void setup_uart(void)
{
	s3k_cap_t pmp_uart
	    = s3k_mk_pmp(s3k_napot_encode(0x10000000, 0x20), 0x3);
	s3k_cap_derive(2, 8, pmp_uart);
	s3k_pmp_load(8, 1);
	s3k_sync();
}

void setup_time(void)
{
	s3k_cap_t boot_time0 = s3k_mk_time(0, 0, 1);
	s3k_cap_t system_time = s3k_mk_time(0, 1, NSLOT);
	error(s3k_cap_derive(HART0_TIME, 16, boot_time0));
	error(s3k_cap_derive(HART0_TIME, 10, system_time));
	s3k_sleep(0);
}

void scheduler(void)
{
	int system_time_cap = 10;
	s3k_sleep(0);

	int i = 0;
	while (1) {
		s3k_sleep(0);
		uint64_t start_time = s3k_get_time();
		error(s3k_cap_revoke(system_time_cap));
		int end = (i++ % 40) + 10;
		s3k_cap_derive(system_time_cap, 26, s3k_mk_time(0, 1, end));
		s3k_cap_derive(system_time_cap, 27, s3k_mk_time(0, end, 50));
		s3k_cap_derive(system_time_cap, 28, s3k_mk_time(0, 50, NSLOT));
		s3k_mon_suspend(MONITOR, PID_COMM);
		s3k_mon_suspend(MONITOR, PID_FCS);
		s3k_mon_suspend(MONITOR, PID_NAV);
		s3k_mon_cap_send(MONITOR, 26, PID_COMM, 3);
		s3k_mon_cap_send(MONITOR, 27, PID_FCS, 3);
		s3k_mon_cap_send(MONITOR, 28, PID_NAV, 3);
		s3k_mon_resume(MONITOR, PID_COMM);
		s3k_mon_resume(MONITOR, PID_FCS);
		s3k_mon_resume(MONITOR, PID_NAV);
		uint64_t end_time = s3k_get_time();
		serio_printf("scheduling cost: %D\n", end_time - start_time);
	}
}

int main(void)
{
	setup_uart();

	serio_printf("initializing comm\n");
	setup_process(PID_COMM, 0x80020000);

	serio_printf("initializing fcs\n");
	setup_process(PID_FCS, 0x80030000);

	serio_printf("initializing nav\n");
	setup_process(PID_NAV, 0x80040000);

	serio_printf("initialization complete\n");

	setup_time();
	scheduler();

	return 0;
}
