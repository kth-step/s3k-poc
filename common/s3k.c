#include "s3k.h"

// Syscall
uint64_t s3k_getpid(void)
{
	register uint64_t a0 __asm__("a0");
	register uint64_t a7 __asm__("a7") = S3K_SYSCALL_GETPID;
	__asm__ volatile("ecall" : "=r"(a0) : "r"(a7));
	return a0;
}

uint64_t s3k_getreg(enum s3k_reg reg)
{
	register uint64_t a0 __asm__("a0") = reg;
	register uint64_t a7 __asm__("a7") = S3K_SYSCALL_GETREG;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a7));
	return a0;
}

uint64_t s3k_setreg(enum s3k_reg reg, uint64_t val)
{
	register uint64_t a0 __asm__("a0") = reg;
	register uint64_t a1 __asm__("a1") = val;
	register uint64_t a7 __asm__("a7") = S3K_SYSCALL_SETREG;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a7));
	return a0;
}

void s3k_yield(void)
{
	register uint64_t a7 __asm__("a7") = S3K_SYSCALL_YIELD;
	__asm__("ecall" : : "r"(a7));
}

union s3k_cap s3k_getcap(uint64_t i)
{
	register uint64_t a0 __asm__("a0") = i;
	register uint64_t a1 __asm__("a1");
	register uint64_t a7 __asm__("a7") = S3K_SYSCALL_GETCAP;
	__asm__ volatile("ecall" : "+r"(a0), "=r"(a1) : "r"(a7));
	return (union s3k_cap){.raw = a0};
}

enum s3k_excpt s3k_movcap(uint64_t i, uint64_t j)
{
	register uint64_t a0 __asm__("a0") = i;
	register uint64_t a1 __asm__("a1") = j;
	register uint64_t a7 __asm__("a7") = S3K_SYSCALL_MOVCAP;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a7));
	return a0;
}

enum s3k_excpt s3k_delcap(uint64_t i)
{
	register uint64_t a0 __asm__("a0") = i;
	register uint64_t a7 __asm__("a7") = S3K_SYSCALL_DELCAP;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a7));
	return a0;
}

enum s3k_excpt s3k_revcap(uint64_t i)
{
	register uint64_t a0 __asm__("a0") = i;
	register uint64_t a7 __asm__("a7") = S3K_SYSCALL_REVCAP;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a7));
	return a0;
}

enum s3k_excpt s3k_drvcap(uint64_t i, uint64_t j, union s3k_cap cap)
{
	register uint64_t a0 __asm__("a0") = i;
	register uint64_t a1 __asm__("a1") = j;
	register uint64_t a2 __asm__("a2") = cap.raw;
	register uint64_t a7 __asm__("a7") = S3K_SYSCALL_DRVCAP;
	__asm__("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a7));
	return a0;
}

enum s3k_excpt s3k_recv(void);
enum s3k_excpt s3k_send(void);
enum s3k_excpt s3k_sendrecv(void);

enum s3k_excpt s3k_msuspend(uint64_t i, uint64_t pid)
{
	register uint64_t a0 __asm__("a0") = i;
	register uint64_t a1 __asm__("a1") = pid;
	register uint64_t a7 __asm__("a7") = S3K_SYSCALL_MSUSPEND;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a7));
	return a0;
}

enum s3k_excpt s3k_mresume(uint64_t i, uint64_t pid)
{
	register uint64_t a0 __asm__("a0") = i;
	register uint64_t a1 __asm__("a1") = pid;
	register uint64_t a7 __asm__("a7") = S3K_SYSCALL_MRESUME;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a7));
	return a0;
}

enum s3k_excpt s3k_mgetreg(uint64_t i, uint64_t pid, enum s3k_reg reg, uint64_t *val)
{
	register uint64_t a0 __asm__("a0") = i;
	register uint64_t a1 __asm__("a1") = pid;
	register uint64_t a2 __asm__("a2") = reg;
	register uint64_t a7 __asm__("a7") = S3K_SYSCALL_MGETREG;
	__asm__ volatile("ecall" : "+r"(a0), "+r"(a1) : "r"(a2), "r"(a7));
	if (a0 == S3K_EXCPT_NONE)
		*val = a1;
	return a0;
}

enum s3k_excpt s3k_msetreg(uint64_t i, uint64_t pid, enum s3k_reg reg, uint64_t val)
{
	register uint64_t a0 __asm__("a0") = i;
	register uint64_t a1 __asm__("a1") = pid;
	register uint64_t a2 __asm__("a2") = reg;
	register uint64_t a3 __asm__("a3") = val;
	register uint64_t a7 __asm__("a7") = S3K_SYSCALL_MSETREG;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3), "r"(a7));
	return a0;
}

enum s3k_excpt s3k_mgetcap(uint64_t i, uint64_t pid, uint64_t j, union s3k_cap *cap)
{
	register uint64_t a0 __asm__("a0") = i;
	register uint64_t a1 __asm__("a1") = pid;
	register uint64_t a2 __asm__("a2") = j;
	register uint64_t a7 __asm__("a7") = S3K_SYSCALL_MGETCAP;
	__asm__ volatile("ecall" : "+r"(a0), "+r"(a1) : "r"(a2), "r"(a7));
	if (a0 == S3K_EXCPT_NONE)
		cap->raw = a1;
	return a0;
}

enum s3k_excpt s3k_mgivecap(uint64_t i, uint64_t pid, uint64_t src, uint64_t dst)
{
	register uint64_t a0 __asm__("a0") = i;
	register uint64_t a1 __asm__("a1") = pid;
	register uint64_t a2 __asm__("a2") = src;
	register uint64_t a3 __asm__("a3") = dst;
	register uint64_t a7 __asm__("a7") = S3K_SYSCALL_MGIVECAP;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3), "r"(a7));
	return a0;
}

enum s3k_excpt s3k_mtakecap(uint64_t i, uint64_t pid, uint64_t src, uint64_t dst)
{
	register uint64_t a0 __asm__("a0") = i;
	register uint64_t a1 __asm__("a1") = pid;
	register uint64_t a2 __asm__("a2") = src;
	register uint64_t a3 __asm__("a3") = dst;
	register uint64_t a7 __asm__("a7") = S3K_SYSCALL_MTAKECAP;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3), "r"(a7));
	return a0;
}

union s3k_cap s3k_time(uint64_t hartid, uint64_t begin, uint64_t free, uint64_t end)
{
	return (union s3k_cap){.time = {S3K_CAPTY_TIME, hartid, begin, free, end}};
}

union s3k_cap s3k_memory(uint64_t begin, uint64_t free, uint64_t end, uint64_t offset,
			 uint64_t lrwx)
{
	return (union s3k_cap){.memory = {S3K_CAPTY_MEMORY, begin, free, end, offset, lrwx}};
}

union s3k_cap s3k_pmp(uint64_t cfg, uint64_t addr)
{
	return (union s3k_cap){.pmp = {S3K_CAPTY_PMP, cfg, addr}};
}

union s3k_cap s3k_monitor(uint64_t begin, uint64_t free, uint64_t end)
{
	return (union s3k_cap){.monitor = {S3K_CAPTY_MONITOR, begin, free, end}};
}

union s3k_cap s3k_channel(uint64_t begin, uint64_t free, uint64_t end)
{
	return (union s3k_cap){.channel = {S3K_CAPTY_CHANNEL, begin, free, end}};
}

union s3k_cap s3k_socket(uint64_t port, uint64_t tag)
{
	return (union s3k_cap){.socket = {S3K_CAPTY_SOCKET, port, tag}};
}

bool s3k_time_derive_time(struct s3k_time parent, struct s3k_time child)
{
	return parent.free <= child.begin && child.end <= parent.end && child.begin == child.free &&
	       child.begin < child.end;
}

bool s3k_memory_derive_memory(struct s3k_memory parent, struct s3k_memory child)
{
	return parent.free <= child.begin && child.end <= parent.end && child.begin == child.free &&
	       child.begin < child.end && (parent.lrwx & child.lrwx) == child.lrwx &&
	       (parent.lrwx & 0x8) == 0;
}

bool s3k_monitor_derive_monitor(struct s3k_monitor parent, struct s3k_monitor child)
{
	return parent.free <= child.begin && child.end <= parent.end && child.begin == child.free &&
	       child.begin < child.end;
}

bool s3k_channel_derive_channel(struct s3k_channel parent, struct s3k_channel child)
{
	return parent.free <= child.begin && child.end <= parent.end && child.begin == child.free &&
	       child.begin < child.end;
}

bool s3k_channel_derive_socket(struct s3k_channel parent, struct s3k_socket child)
{
	return parent.free <= child.port && child.port < parent.end && child.tag == 0;
}

bool s3k_socket_derive_socket(struct s3k_socket parent, struct s3k_socket child)
{
	return parent.port == child.port && parent.tag == 0 && child.tag > 0;
}

bool s3k_time_derive(union s3k_cap parent, union s3k_cap child)
{
	return parent.type == S3K_CAPTY_TIME && child.type == S3K_CAPTY_TIME &&
	       s3k_time_derive_time(parent.time, child.time);
}

bool s3k_memory_derive(union s3k_cap parent, union s3k_cap child)
{
	return parent.type == S3K_CAPTY_MEMORY && child.type == S3K_CAPTY_MEMORY &&
	       s3k_memory_derive_memory(parent.memory, child.memory);
}

bool s3k_monitor_derive(union s3k_cap parent, union s3k_cap child)
{
	return parent.type == S3K_CAPTY_MONITOR && child.type == S3K_CAPTY_MONITOR &&
	       s3k_monitor_derive_monitor(parent.monitor, child.monitor);
}

bool s3k_channel_derive(union s3k_cap parent, union s3k_cap child)
{
	return (parent.type == S3K_CAPTY_CHANNEL && child.type == S3K_CAPTY_CHANNEL &&
		s3k_channel_derive_channel(parent.channel, child.channel)) ||
	       (parent.type == S3K_CAPTY_CHANNEL && child.type == S3K_CAPTY_SOCKET &&
		s3k_channel_derive_socket(parent.channel, child.socket));
}

bool s3k_socket_derive(union s3k_cap parent, union s3k_cap child)
{
	return (parent.type == S3K_CAPTY_SOCKET && child.type == S3K_CAPTY_SOCKET &&
		s3k_socket_derive_socket(parent.socket, child.socket));
}
