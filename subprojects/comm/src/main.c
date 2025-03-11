#include "s3k.h"
#include "serio.h"
#include "serio/uart/ns16550a.h"

SERIOFILE _uartfile = SERIO_UART_NS16550A(0x10000000);
SERIOFILE *const _serio_out = &_uartfile;

int main(void)
{
	s3k_cap_derive(2, 10,
		       s3k_mk_pmp(s3k_napot_encode(0x10000000, 0x20), 0x3));
	s3k_pmp_load(10, 1);
	s3k_sync();

	serio_printf("hello from proc %D\n", s3k_get_pid());
	return 0;
}
