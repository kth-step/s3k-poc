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

bool capman_delcap(uint64_t idx) {
	if (caps[idx].raw) {
		s3k_delcap(idx);
		caps[idx].raw = 0;
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
	alt_printf("time{hartid=0x%X,begin=0x%X,end=0x%X,free=0x%X}\n", time.hartid, time.begin, time.end, time.free);
}

static void _dump_memory(struct s3k_memory mem)
{
	uint64_t begin = ((uint64_t)mem.offset << 27) + ((uint64_t)mem.begin << 12);
	uint64_t end =   ((uint64_t)mem.offset << 27) + ((uint64_t)mem.end << 12);
	uint64_t free =  ((uint64_t)mem.offset << 27) + ((uint64_t)mem.free << 12);
	alt_printf(
	    "memory{begin=0x%X,end=0x%X,free=0x%x,lock=0x%X,rwx=0x%X}"
	    "\n",
	    begin, end, free, mem.lock, mem.rwx);
}

static void _dump_pmp(struct s3k_pmp pmp)
{
	uint64_t begin = s3k_pmp_napot_begin(pmp.addr);
	uint64_t end = s3k_pmp_napot_end(pmp.addr);
	uint64_t rwx = pmp.cfg & 0x7;
	alt_printf("pmp{begin=0x%X,end=0x%X,rwx=0x%X}\n", begin, end, rwx);
}

static void _dump_monitor(struct s3k_monitor mon)
{
	alt_printf("monitor{begin=0x%X,end=0x%X,free=0x%X}\n", mon.begin, mon.end, mon.free);
}

static void _dump_channel(struct s3k_channel chan)
{
	alt_printf("channel{begin=0x%X,end=0x%X,free=0x%X}\n", chan.begin, chan.end, chan.free);
}

void capman_dump(union s3k_cap cap)
{
	switch (cap.type) {
	case S3K_CAPTY_NONE:
		break;
	case S3K_CAPTY_TIME:
		_dump_time(cap.time);
		break;
	case S3K_CAPTY_MEMORY:
		_dump_memory(cap.memory);
		break;
	case S3K_CAPTY_PMP:
		_dump_pmp(cap.pmp);
		break;
	case S3K_CAPTY_MONITOR:
		_dump_monitor(cap.monitor);
		break;
	case S3K_CAPTY_CHANNEL:
		_dump_channel(cap.channel);
		break;
	default:
		alt_printf("0x%X\n", cap.raw);
		break;
	}
}

void capman_dump_all(void)
{
	for (int i = 0; i < NCAP; i++) {
		if (caps[i].raw == 0)
			continue;
		alt_printf("0x%x: ", i);
		capman_dump(caps[i]);
	}
}

int capman_find_free(void)
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

static int find_time_derive(union s3k_cap cap)
{
	for (int i = NCAP - 1; i >= 0; --i) {
		if (s3k_time_derive(caps[i], cap))
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

bool capman_derive_pmp(int idx, uint64_t begin, uint64_t end, uint64_t rwx)
{
	uint64_t addr = s3k_pmp_napot_addr(begin, end);
	union s3k_cap pmp = s3k_pmp(addr, rwx);
	if (begin != s3k_pmp_napot_begin(addr) || end != s3k_pmp_napot_end(addr))
		return false;

	uint64_t i = find_memory_derive(pmp);
	if (s3k_drvcap(i, idx, pmp) == S3K_EXCPT_NONE) {
		caps[i] = s3k_getcap(i);
		caps[idx] = s3k_getcap(idx);
		ncaps++;
		return true;
	}
	return false;
}

bool capman_derive_mem(int idx, uint64_t begin, uint64_t end, uint64_t rwx)
{
	uint64_t _offset = begin >> 27;
	uint64_t _begin = (begin >> 12) & 0xFFFF;
	uint64_t _end = (end >> 12) & 0xFFFF;
	union s3k_cap mem = s3k_memory(_begin, _end, _offset, rwx);
	if (_begin > _end || ((begin >> 27) != (end >> 27)))
		return false;

	uint64_t i = find_memory_derive(mem);
	if (s3k_drvcap(i, idx, mem) == S3K_EXCPT_NONE) {
		caps[i] = s3k_getcap(i);
		caps[idx] = s3k_getcap(idx);
		ncaps++;
		return true;
	}
	return false;
}

bool capman_derive_time(int idx, uint64_t hartid, uint64_t begin, uint64_t end)
{
	union s3k_cap mem = s3k_time(hartid, begin, end);
	if (begin > end)
		return false;

	uint64_t i = find_time_derive(mem);
	if (s3k_drvcap(i, idx, mem) == S3K_EXCPT_NONE) {
		caps[i] = s3k_getcap(i);
		caps[idx] = s3k_getcap(idx);
		ncaps++;
		return true;
	}
	return false;
}

int capman_find_monitor(uint64_t pid)
{
	for (int i = 0; i < NCAP; i++) 
	{
		if (caps[i].type == S3K_CAPTY_MONITOR) {
			struct s3k_monitor mon = caps[i].monitor;
			if (mon.free <= pid && pid < mon.end) {
				return i;
			}
		}
	}
	return -1;
}

bool capman_mresume(uint64_t pid)
{
	int moncap = capman_find_monitor(pid);
	if (moncap < 0)
		return false;
	s3k_mresume(moncap, pid);
	return true;
}

bool capman_msuspend(uint64_t pid)
{
	int moncap = capman_find_monitor(pid);
	if (moncap < 0)
		return false;
	s3k_msuspend(moncap, pid);
	return true;
}

bool capman_mgivecap(uint64_t pid, uint64_t src, uint64_t dest)
{
	int moncap = capman_find_monitor(pid);
	if (moncap < 0)
		return false;
	int excpt = s3k_mgivecap(moncap, pid, src, dest);
	if (excpt == S3K_CAPTY_NONE) {
		caps[src].raw = 0;
		return true;
	}
	return false;
}

bool capman_mtakecap(uint64_t pid, uint64_t src, uint64_t dest)
{
	int moncap = capman_find_monitor(pid);
	if (moncap < 0)
		return false;
	int excpt = s3k_mtakecap(moncap, pid, src, dest);
	if (excpt == S3K_CAPTY_NONE) {
		caps[dest] = s3k_getcap(dest);
		return true;
	}
	return false;
}

bool capman_msetreg(uint64_t pid, uint64_t reg, uint64_t val) 
{
	int moncap = capman_find_monitor(pid);
	if (moncap < 0)
		return false;
	int excpt = s3k_msetreg(moncap, pid, reg, val);
	return (excpt == S3K_CAPTY_NONE);
}

bool capman_mgetreg(uint64_t pid, uint64_t reg, uint64_t *val) 
{
	int moncap = capman_find_monitor(pid);
	if (moncap < 0)
		return false;
	int excpt = s3k_mgetreg(moncap, pid, reg, val);
	return (excpt == S3K_CAPTY_NONE);
}
