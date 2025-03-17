#include "s3k.h"
#include "serio.h"
#include "serio/uart/ns16550a.h"

SERIOFILE _uartfile = SERIO_UART_NS16550A(0x10000000);
SERIOFILE *const _serio_out = &_uartfile;

int main(void)
{
	int pid = s3k_get_pid();
	serio_printf("hello from proc %D\n", pid);
	s3k_sleep(0);
	while(1) {
		uint64_t time = s3k_get_time();
		uint64_t timeout = s3k_get_timeout();
		serio_printf("%D: %D - %D\n", pid, time, timeout);
		s3k_sleep(0);
	}
	return 0;
}
