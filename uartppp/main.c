#include <stddef.h>
#include <stdint.h>

#include "altio.h"
#include "ppp.h"
#include "ring_buffer.h"
#include "s3k.h"

char *buffer;
size_t buffer_size;

const char *msg_ready = "uart_ready";
const char *msg_long = "frame too long";

size_t strlen(const char *s)
{
	size_t len = 0;
	while (s[len] != '\0')
		len++;
	return len;
}

void setup(char *_buf, size_t _size)
{
	alt_puts("setup uart");
}

void loop(void)
{
	s3k_yield();
}
