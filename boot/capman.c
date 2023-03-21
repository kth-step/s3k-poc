#include "capman.h"

#include <stdbool.h>
#include <stdint.h>

#include "../config.h"
#include "s3k.h"

static uint64_t ncaps;
static union s3k_cap caps[NCAP];

void capman_init(void)
{
	capman_update();
}

void capman_update(void)
{
	ncaps = 0;
	for (int i = 0; i < NCAP; i++) {
		caps[i] = s3k_getcap(i);
		if (caps[i].type != S3K_CAPTY_NONE)
			ncaps++;
	}
}

static int find_free(void)
{
	for (int i = NCAP - 1; i >= 0; --i) {
		if (caps[i].type == S3K_CAPTY_NONE)
			return i;
	}
	return -1;
}

static int find_memory_derive(union s3k_cap cap)
{
	for (int i = NCAP - 1; i >= 0; --i) {
		if (s3k_memory_derive(caps[i], cap))
			return i;
	}
	return -1;
}

void capman_setpmp(uint8_t pmp[8])
{
	uint64_t pmpreg = 0;
	for (int i = 0; i < 8; ++i) {
		pmpreg |= ((uint64_t)pmp[i]) << (i * 8);
	}
	s3k_setreg(S3K_REG_PMP, pmpreg);
}

void capman_getpmp(uint8_t pmp[8])
{
	uint64_t pmpreg = s3k_getreg(S3K_REG_PMP);
	for (int i = 0; i < 8; ++i) {
		pmp[i] = (pmpreg >> (i * 8));
	}
}

int capman_derive_pmp(uint64_t begin, uint64_t end, uint64_t rwx)
{
	uint64_t addr = s3k_pmp_napot_addr(begin, end);
	union s3k_cap pmp = s3k_pmp(addr, rwx);
	if (begin != s3k_pmp_napot_begin(addr) || end != s3k_pmp_napot_end(addr))
		return -1;

	uint64_t i = find_memory_derive(pmp);
	uint64_t j = find_free();
	if (s3k_drvcap(i, j, pmp) == S3K_EXCPT_NONE) {
		caps[i] = s3k_getcap(i);
		caps[j] = s3k_getcap(j);
		ncaps++;
		return j;
	}
	return -1;
}

int capman_derive_mem(uint64_t begin, uint64_t end, uint64_t rwx)
{
	uint64_t _offset = begin >> 27;
	uint64_t _begin = (begin >> 12) & 0xFFFF;
	uint64_t _end = (end >> 12) & 0xFFFF;
	union s3k_cap mem = s3k_memory(_begin, _end, _offset, rwx);
	if (_begin > _end || ((begin >> 27) != (end >> 27)))
		return -1;

	uint64_t i = find_memory_derive(mem);
	uint64_t j = find_free();
	if (s3k_drvcap(i, j, mem) == S3K_EXCPT_NONE) {
		caps[i] = s3k_getcap(i);
		caps[j] = s3k_getcap(j);
		ncaps++;
		return j;
	}
	return -1;
}
