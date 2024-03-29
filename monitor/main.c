#include <stdbool.h>
#include <string.h>

#include "altio.h"
#include "s3k.h"

void setup(void)
{
	alt_puts("monitor\n");
	s3k_revcap(0x18);
}

void loop(void)
{
	s3k_yield();
	uint64_t tag;
	uint64_t buf[4] = {0x1, 0x2, 0x4, 0x8};
	s3k_sendrecv(0xa, 0x9, buf, -1ull, -1ull, &tag);
	alt_printf("monitor: tag=%X msg=%X,%X,%X,%X\n", tag, buf[0], buf[1], buf[2], buf[3]);
}
