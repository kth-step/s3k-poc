#include "s3k.h"
#include "serio.h"

#define N_COMPONENTS 2
#define SYSTEM_SLOTS (NSLOT - 2)
#define RUNS 100

enum {
	BOOT_PMP = 0,
	RAM_MEM = 1,
	UART_MEM = 2,
	TIMER_MEM = 3,
	HART0_TIME = 4,
	MONITOR = 5,
	CHANNEL = 6,
};

#define error(err)                                                \
	do {                                                      \
		if (err) {                                        \
			serio_printf("Error %s at line %d\n",     \
				     s3k_err2str(err), __LINE__); \
		}                                                 \
	} while (0);

uint64_t csrr_cycle()
{
	uint64_t tmp;
	__asm__ volatile("rdcycle %0" : "=r"(tmp));
	return tmp;
}

void setup_uart(void)
{
	s3k_cap_t pmp_uart
	    = s3k_mk_pmp(s3k_napot_encode(0x03002000, 0x20), 0x3);
	s3k_cap_derive(2, 8, pmp_uart);
	s3k_pmp_load(8, 1);
	s3k_sync();
}

void setup_memory(int pid, uint64_t addr)
{
	s3k_cap_t uart = s3k_mk_pmp(s3k_napot_encode(0x03002000, 0x20), 0x3);
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

	// Start process
	error(s3k_mon_resume(MONITOR, pid));
	s3k_mon_yield(MONITOR, pid);
}

void setup_ipc(void)
{
	uint64_t channel = 0;
	uint64_t mode = S3K_IPC_YIELD;
	uint64_t permission = S3K_IPC_SDATA | S3K_IPC_CDATA | S3K_IPC_SCAP
			      | S3K_IPC_CCAP;
	s3k_cap_t server = s3k_mk_socket(channel, mode, permission, 0);
	s3k_cap_t client = s3k_mk_socket(channel, mode, permission, 1);
	error(s3k_cap_derive(CHANNEL, 12, server));
	error(s3k_cap_derive(12, 13, client));
	error(s3k_mon_cap_send(MONITOR, 12, 1, 4));
	error(s3k_mon_cap_send(MONITOR, 13, 2, 4));
}

void setup_time(void)
{
	error(s3k_time_derive(HART0_TIME, 10, SYSTEM_SLOTS, 0));
	error(s3k_time_derive(HART0_TIME, 16, NSLOT - SYSTEM_SLOTS, 1));
	error(s3k_time_delete(HART0_TIME));
	s3k_mon_time_derive(MONITOR, 10, 2, 3, SYSTEM_SLOTS, 1);
}

int main(void)
{
	setup_uart();
	serio_putstr("UART setup\n");

	s3k_sleep(0);

	setup_ipc();
	setup_memory(1, 0x80010000);
	setup_memory(2, 0x80020000);

	uint64_t wcet_worst = 0;
	s3k_sleep(0);
	setup_time();
	s3k_get_wcet();
	for (int i = 0; i <= RUNS; ++i) {
		s3k_sleep(0);
		uint64_t wcet = s3k_get_wcet();
		serio_printf("WCET: %D\n", wcet);
		if (wcet > wcet_worst)
			wcet_worst = wcet;
	}

	s3k_time_revoke(10);
	serio_putstr("Tests completed!\n");
	serio_printf("WCET: %D\n", wcet_worst);

	while (1)
		s3k_sleep(0);
	return 0;
}
