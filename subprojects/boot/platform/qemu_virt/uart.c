#include "serio/ns16550a.h"

int serio_putchar(int c)
{
	return serio_ns16550a_putchar(c, (void *)0x10000000);
}
