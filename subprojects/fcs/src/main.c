#include "s3k.h"
#include "serio.h"
#include "serio/uart/ns16550a.h"

SERIOFILE _uartfile = SERIO_UART_NS16550A(0x10000000);
SERIOFILE *const _serio_out = &_uartfile;

int main(void)
{
	serio_printf("hello from proc %D\n", s3k_get_pid());
	return 0;
}
