#include "s3k.h"

#define UART ((void *)0x10000000)

int putchar(int c)
{
	volatile char *uart = UART;
	*uart = c;
	return (char)c;
}

int putstr(char *s)
{
	int i = 0;
	while (s[i] != '\0') {
		putchar(s[i++]);
	}
	return i;
}

int main(void)
{
	s3k_cap_derive(2, 10,
		       s3k_mk_pmp(s3k_napot_encode(0x10000000, 0x20), 0x3));
	s3k_pmp_load(10, 1);
	s3k_sync();

	while (1) {
		putstr("hello, world\n");
	}
	return s3k_get_pid();
}
