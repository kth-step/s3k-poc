#include "s3k.h"
#include "sched.h"
#include "serio.h"

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
	__asm__ volatile ("rdcycle %0": "=r"(tmp));
	return tmp;
}

void setup_process(int pid, uint64_t addr)
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

void setup_uart(void)
{
	s3k_cap_t pmp_uart
	    = s3k_mk_pmp(s3k_napot_encode(0x03002000, 0x20), 0x3);
	s3k_cap_derive(2, 8, pmp_uart);
	s3k_pmp_load(8, 1);
	s3k_sync();
}

void setup_time(void)
{
	error(s3k_time_derive(HART0_TIME, 10, SYSTEM_SLOTS, 0));
	error(s3k_time_derive(HART0_TIME, 16, NSLOT - SYSTEM_SLOTS, 1));
	error(s3k_cap_delete(HART0_TIME));
	s3k_sleep(0);
}

unsigned long xorshift()
{
	static unsigned long x = 123456781;
	static unsigned long y = 362436069;
	static unsigned long z = 521288629;
	static unsigned long w = 88675123;
	unsigned long t = x ^ (x << 11);
	x = y;
	y = z;
	z = w;
	w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
	return w;
}

void variable_gen(int v[N_VARIABLES])
{
	for (int i = 0; i < N_VARIABLES; ++i) {
		v[i] = (xorshift() % (V_MAX - V_MIN + 1)) + V_MIN;
	}
}

void apply_schedule(int s[N_COMPONENTS], int components)
{
	int system_time_cap = 10;
	int end;
	int start = 0;

	s3k_time_revoke(system_time_cap);
	for (int i = 1; i < components; ++i) {
		end = start + s[i - 1];
		if (start < end && end <= SYSTEM_SLOTS) {
			s3k_mon_time_derive(MONITOR, system_time_cap, i, 4,
					    end - start, 1);
		}
		start = end;
	}
	if (SYSTEM_SLOTS - start > 0)
		s3k_mon_time_derive(MONITOR, system_time_cap, components, 4,
		      SYSTEM_SLOTS - start, 1);
}

uint64_t run_test(int runs, int components)
{
	int v[N_VARIABLES];
	int s[N_COMPONENTS];
	uint64_t total_time = 0;
	for (int i = 0; i < RUNS; ++i) {
		s3k_sleep(0);
		//__asm__ volatile (".word 0xb");
		variable_gen(v);
		sched_calc(v, s);
		uint64_t start_time = csrr_cycle();
		apply_schedule(s, components);
		uint64_t end_time = csrr_cycle();
		total_time += end_time - start_time;
	}
	s3k_time_revoke(10);
	return total_time / runs;
}

int main(void)
{
	setup_uart();

	for (int i = 1; i <= N_COMPONENTS; ++i) {
		setup_process(i, 0x80010000 + 0x10000 * i);
	}
	serio_printf("initialization complete\n");

	setup_time();

	uint64_t stats[N_COMPONENTS];

	for (int i = 2; i <= N_COMPONENTS; ++i) {
		stats[i-1] = run_test(RUNS, i);
	}

	serio_putstr("Tests completed!\n");
	serio_putstr("comps\tcost\n");
	for (int i = 2; i <= N_COMPONENTS; ++i) {
		serio_printf("%d\t%D\n", i, stats[i-1]);
	}

	while (1)
		s3k_sleep(0);
	return 0;
}
