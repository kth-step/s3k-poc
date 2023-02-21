#include <stddef.h>

#include "ppp.h"

char *buf;
size_t size;

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
	buf = _buf;
	size = _size;
	ppp_send(msg_ready, strlen(msg_long));
}

void loop(void)
{
	while (1) {
		int len = ppp_recv(buf, size);
		if (len > 0)
			ppp_send(buf, len);
		else
			ppp_send(msg_long, strlen(msg_long));
	}
}
