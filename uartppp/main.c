#include <stddef.h>
#include <stdint.h>

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
	buffer = _buf;
	buffer_size = _size;
	ppp_send(msg_ready, strlen(msg_long));
}

void loop(void)
{
	while (1) {
		int len = ppp_recv(buffer, buffer_size);
		if (len == -1)
			ppp_send(buffer, buffer_size);
		else
			ppp_send(buffer, len);
		s3k_yield();
	}
}
