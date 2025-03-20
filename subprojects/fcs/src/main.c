#include "s3k.h"
#include "serio.h"
#include "serio/ns16550a.h"

int serio_putchar(int c)
{
	return serio_ns16550a_putchar(c, (void*)0x10000000);
}

int main(void)
{
	int pid = s3k_get_pid();
	serio_printf("hello from proc %D\n", pid);
	s3k_sleep(0);
	while (1) {
		uint64_t time = s3k_get_time();
		uint64_t timeout = s3k_get_timeout();
		serio_printf("%D: %D\n", pid, timeout - time);
		s3k_sleep(0);
	}
	return 0;
}
