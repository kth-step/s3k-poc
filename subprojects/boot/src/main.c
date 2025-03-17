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

void error(int err)
{
	if (err) {
		serio_printf("Error %s\n", s3k_err2str(err));
		while (1) {}
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
	// Set PC
	error(s3k_mon_reg_write(MONITOR, pid, S3K_REG_PC, addr));
	// Done?
	error(s3k_mon_resume(MONITOR, pid));
}

void setup_uart(void)
{
	s3k_cap_t pmp_uart = s3k_mk_pmp(s3k_napot_encode(0x10000000, 0x20), 0x3);
	s3k_cap_derive(2,8,pmp_uart);
	s3k_pmp_load(8, 2);
	s3k_sync();
}

int main(void)
{
	setup_uart();
	serio_printf("boot loaded initialized\n");

	serio_printf("initializing comm\n");
	setup_process(1, 0x80020000);

	serio_printf("initializing fcs\n");
	setup_process(2, 0x80030000);

	serio_printf("initializing nav\n");
	setup_process(3, 0x80040000);

	serio_printf("initialization complete\n");
	while (1) {
		s3k_mon_yield(MONITOR, 1);
		s3k_mon_yield(MONITOR, 2);
		s3k_mon_yield(MONITOR, 3);
	}
	return 0;
}
