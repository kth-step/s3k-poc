#include "altio.h"
#include "ppp.h"
#include "ring_buffer.h"
#include "s3k.h"

#include <stddef.h>
#include <stdint.h>

uint64_t buf[4] = {0,0,0,0};

void setup(char *_buf, size_t _size)
{
	alt_puts("setup uart");
}

void recv_msg(uint8_t buf[32])
{
	for(int i = 0; i < 32; i++)
		buf[i] = i + 1;
}

void send_msg(uint8_t buf[32])
{
	for(int i = 0; i < 32; ++i)
		alt_putchar(buf[i]);
}

void loop(void)
{
	uint64_t tag;

	// Receive a message.
	recv_msg((uint8_t*)buf);

	// Send message to monitor and get response
	while (s3k_sendrecv(3, buf, &tag)) {
	}

	// Send a message if non-zero response
	if (buf[0] | buf[1] | buf[2] | buf[3])
		send_msg((uint8_t*)buf);
}
