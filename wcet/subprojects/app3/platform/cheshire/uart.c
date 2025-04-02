#include "serio/ti16750.h"

int serio_putchar(int c)
{
	if (c == '\n')
		serio_ti16750_putchar('\r', (void *)0x03002000);
	return serio_ti16750_putchar(c, (void *)0x03002000);
}
