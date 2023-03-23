#include "ring_buffer.h"
#include "altio.h"
#define SHARED 0x80030000

struct ring_buffer *rb = (struct ring_buffer*)SHARED;

void setup(void)
{
	alt_puts("app0 running");
}

static char konsonanter[] = "BCDFGHJKLMNPQRSTVWXZbcdfghjklmnpqrstvwxz";

int member(char c, char *str)
{
	do {
		if (c == *str)
			return 1;
	} while (*str++);
	return 0;
}

void loop(void)
{
	char c;
	while (!ring_buffer_pop(rb, &c));	
	if (c == '\r')
	alt_putchar('\n');
	if (member(c, konsonanter)) {
		alt_putchar(c);
		alt_putchar('o');
		if ('A' <= c && c <= 'Z')
			alt_putchar(c + ('a' - 'A'));
		else
			alt_putchar(c);
	} else {
		alt_putchar(c);
	}
}
