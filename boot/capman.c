#include "capman.h"

#include <stdbool.h>
#include <stdint.h>

#include "../config.h"
#include "altio.h"
#include "s3k.h"

static uint64_t ncaps;
static union s3k_cap caps[NCAP];

void capman_init(void)
{
	capman_update();
}

union s3k_cap capman_get(uint64_t idx)
{
	return caps[idx];
}

bool capman_move(uint64_t src, uint64_t dst)
{
	if (caps[src].raw == 0 || caps[dst].raw != 0)
		return false;
	if (s3k_movcap(src, dst) != S3K_EXCPT_NONE) {
		caps[dst].raw = caps[src].raw;
		caps[src].raw = 0;
		return true;
	}
	return false;
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

void capman_dump(void)
{
	for (int i = 0; i < NCAP; i++) {
		switch (caps[i].type) {
		case S3K_CAPTY_NONE:
			break;
		case S3K_CAPTY_TIME:
			alt_printf("0x%x: time{begin=0x%X,end=0x%X,free=0x%X}\n", i,
				   caps[i].time.begin, caps[i].time.end, caps[i].time.free);
			break;
		case S3K_CAPTY_MEMORY:
			alt_printf(
			    "0x%x: "
			    "memory{begin=0x%X,end=0x%X,free=0x%x,offset=0x%X,lock=0x%X,rwx=0x%X}"
			    "\n",
			    i, caps[i].memory.begin, caps[i].memory.end, caps[i].memory.free,
			    caps[i].memory.offset, caps[i].memory.lock, caps[i].memory.rwx);
			break;
		case S3K_CAPTY_PMP:
			alt_printf("0x%x: pmp{addr=0x%X,cfg=0x%X}\n", i, caps[i].pmp.addr,
				   caps[i].pmp.cfg);
			break;
		case S3K_CAPTY_MONITOR:
			alt_printf("0x%x: monitor{begin=0x%X,end=0x%X,free=0x%X}\n", i,
				   caps[i].monitor.begin, caps[i].monitor.end,
				   caps[i].monitor.free);
			break;
		case S3K_CAPTY_CHANNEL:
			alt_printf("0x%x: channel{begin=0x%X,end=0x%X,free=0x%X}\n", i,
				   caps[i].channel.begin, caps[i].channel.end,
				   caps[i].channel.free);
			break;
		default:
			alt_printf("0x%x: 0x%X\n", i, caps[i].raw);
			break;
		}
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
