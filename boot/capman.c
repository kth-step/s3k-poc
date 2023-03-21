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

static void _dump_time(struct s3k_time time)
{
	alt_printf("time{begin=0x%X,end=0x%X,free=0x%X}\n", time.begin, time.end, time.free);
}

static void _dump_memory(struct s3k_memory mem)
{
	alt_printf(
	    "memory{begin=0x%X,end=0x%X,free=0x%x,offset=0x%X,lock=0x%X,rwx=0x%X}"
	    "\n",
	    mem.begin, mem.end, mem.free, mem.offset, mem.lock, mem.rwx);
}

static void _dump_pmp(struct s3k_pmp pmp)
{
	alt_printf("pmp{addr=0x%X,cfg=0x%X}\n", pmp.addr, pmp.cfg);
}

static void _dump_monitor(struct s3k_monitor mon)
{
	alt_printf("monitor{begin=0x%X,end=0x%X,free=0x%X}\n", mon.begin, mon.end, mon.free);
}

static void _dump_channel(struct s3k_channel chan)
{
	alt_printf("channel{begin=0x%X,end=0x%X,free=0x%X}\n", chan.begin, chan.end, chan.free);
}

void capman_dump(void)
{
	for (int i = 0; i < NCAP; i++) {
		if (caps[i].raw == 0)
			continue;
		alt_printf("0x%x: ", i);
		switch (caps[i].type) {
		case S3K_CAPTY_NONE:
			break;
		case S3K_CAPTY_TIME:
			_dump_time(caps[i].time);
			break;
		case S3K_CAPTY_MEMORY:
			_dump_memory(caps[i].memory);
			break;
		case S3K_CAPTY_PMP:
			_dump_pmp(caps[i].pmp);
			break;
		case S3K_CAPTY_MONITOR:
			_dump_monitor(caps[i].monitor);
			break;
		case S3K_CAPTY_CHANNEL:
			_dump_channel(caps[i].channel);
			break;
		default:
			alt_printf("0x%X\n", caps[i].raw);
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
