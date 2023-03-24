#include <stdbool.h>
#include <string.h>

#include "s3k.h"
#include "altio.h"

void setup(void)
{
	s3k_revcap(0x18);
}

void loop(void)
{
	s3k_yield();
}
