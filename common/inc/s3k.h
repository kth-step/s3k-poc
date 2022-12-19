# 0 "api/s3k_call.h"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "api/s3k_call.h"
# 1 "api/../inc/cap.h" 1

typedef enum cap_type {
        CAP_TYPE_EMPTY,
        CAP_TYPE_MEMORY,
        CAP_TYPE_PMP,
        CAP_TYPE_TIME,
        CAP_TYPE_CHANNELS,
        CAP_TYPE_RECEIVER,
        CAP_TYPE_SENDER,
        CAP_TYPE_SERVER,
        CAP_TYPE_CLIENT,
        CAP_TYPE_SUPERVISOR,
        NUM_OF_CAP_TYPES
} cap_type_t;

typedef struct cap {
        unsigned long word0, word1;
} cap_t;

static inline unsigned long pmp_napot_begin(unsigned long addr)
{
        return addr & (addr + 1);
}

static inline unsigned long pmp_napot_end(unsigned long addr)
{
        return addr | (addr + 1);
}

static inline cap_type_t cap_get_type(cap_t cap)
{
        return (cap_type_t)(0xfful & cap.word0);
}

static inline int cap_is_type(cap_t cap, cap_type_t t)
{
        return cap_get_type(cap) == t;
}

static inline cap_t cap_mk_memory(unsigned long begin, unsigned long end, unsigned long rwx, unsigned long free,
                                  unsigned long pmp)
{
        cap_t cap;
        cap.word0 = (unsigned long)CAP_TYPE_MEMORY;
        cap.word1 = 0;
        cap.word0 |= pmp << 8;
        cap.word0 |= rwx << 16;
        cap.word0 |= begin << 24;
        cap.word1 |= free;
        cap.word1 |= end << 32;
        return cap;
}
static inline unsigned long cap_memory_get_begin(cap_t cap)
{
        return (cap.word0 >> 24) & 0xfffffffful;
}
static inline cap_t cap_memory_set_begin(cap_t cap, unsigned long begin)
{
        cap.word0 = (cap.word0 & ~0xffffffff000000ul) | begin << 24;
        return cap;
}
static inline unsigned long cap_memory_get_end(cap_t cap)
{
        return (cap.word1 >> 32) & 0xfffffffful;
}
static inline cap_t cap_memory_set_end(cap_t cap, unsigned long end)
{
        cap.word1 = (cap.word1 & ~0xffffffff00000000ul) | end << 32;
        return cap;
}
static inline unsigned long cap_memory_get_rwx(cap_t cap)
{
        return (cap.word0 >> 16) & 0xfful;
}
static inline cap_t cap_memory_set_rwx(cap_t cap, unsigned long rwx)
{
        cap.word0 = (cap.word0 & ~0xff0000ul) | rwx << 16;
        return cap;
}
static inline unsigned long cap_memory_get_free(cap_t cap)
{
        return cap.word1 & 0xfffffffful;
}
static inline cap_t cap_memory_set_free(cap_t cap, unsigned long free)
{
        cap.word1 = (cap.word1 & ~0xfffffffful) | free << 0;
        return cap;
}
static inline unsigned long cap_memory_get_pmp(cap_t cap)
{
        return (cap.word0 >> 8) & 0xfful;
}
static inline cap_t cap_memory_set_pmp(cap_t cap, unsigned long pmp)
{
        cap.word0 = (cap.word0 & ~0xff00ul) | pmp << 8;
        return cap;
}
static inline cap_t cap_mk_pmp(unsigned long addr, unsigned long rwx)
{
        cap_t cap;
        cap.word0 = (unsigned long)CAP_TYPE_PMP;
        cap.word1 = 0;
        cap.word0 |= rwx << 8;
        cap.word0 |= addr << 16;
        return cap;
}
static inline unsigned long cap_pmp_get_addr(cap_t cap)
{
        return (cap.word0 >> 16) & 0xfffffffful;
}
static inline cap_t cap_pmp_set_addr(cap_t cap, unsigned long addr)
{
        cap.word0 = (cap.word0 & ~0xffffffff0000ul) | addr << 16;
        return cap;
}
static inline unsigned long cap_pmp_get_rwx(cap_t cap)
{
        return (cap.word0 >> 8) & 0xfful;
}
static inline cap_t cap_pmp_set_rwx(cap_t cap, unsigned long rwx)
{
        cap.word0 = (cap.word0 & ~0xff00ul) | rwx << 8;
        return cap;
}
static inline cap_t cap_mk_time(unsigned long hartid, unsigned long begin, unsigned long end, unsigned long free)
{
        cap_t cap;
        cap.word0 = (unsigned long)CAP_TYPE_TIME;
        cap.word1 = 0;
        cap.word0 |= free << 8;
        cap.word0 |= end << 24;
        cap.word0 |= begin << 40;
        cap.word0 |= hartid << 56;
        return cap;
}
static inline unsigned long cap_time_get_hartid(cap_t cap)
{
        return (cap.word0 >> 56) & 0xfful;
}
static inline cap_t cap_time_set_hartid(cap_t cap, unsigned long hartid)
{
        cap.word0 = (cap.word0 & ~0xff00000000000000ul) | hartid << 56;
        return cap;
}
static inline unsigned long cap_time_get_begin(cap_t cap)
{
        return (cap.word0 >> 40) & 0xfffful;
}
static inline cap_t cap_time_set_begin(cap_t cap, unsigned long begin)
{
        cap.word0 = (cap.word0 & ~0xffff0000000000ul) | begin << 40;
        return cap;
}
static inline unsigned long cap_time_get_end(cap_t cap)
{
        return (cap.word0 >> 24) & 0xfffful;
}
static inline cap_t cap_time_set_end(cap_t cap, unsigned long end)
{
        cap.word0 = (cap.word0 & ~0xffff000000ul) | end << 24;
        return cap;
}
static inline unsigned long cap_time_get_free(cap_t cap)
{
        return (cap.word0 >> 8) & 0xfffful;
}
static inline cap_t cap_time_set_free(cap_t cap, unsigned long free)
{
        cap.word0 = (cap.word0 & ~0xffff00ul) | free << 8;
        return cap;
}
static inline cap_t cap_mk_channels(unsigned long begin, unsigned long end, unsigned long free)
{
        cap_t cap;
        cap.word0 = (unsigned long)CAP_TYPE_CHANNELS;
        cap.word1 = 0;
        cap.word0 |= free << 8;
        cap.word0 |= end << 24;
        cap.word0 |= begin << 40;
        return cap;
}
static inline unsigned long cap_channels_get_begin(cap_t cap)
{
        return (cap.word0 >> 40) & 0xfffful;
}
static inline cap_t cap_channels_set_begin(cap_t cap, unsigned long begin)
{
        cap.word0 = (cap.word0 & ~0xffff0000000000ul) | begin << 40;
        return cap;
}
static inline unsigned long cap_channels_get_end(cap_t cap)
{
        return (cap.word0 >> 24) & 0xfffful;
}
static inline cap_t cap_channels_set_end(cap_t cap, unsigned long end)
{
        cap.word0 = (cap.word0 & ~0xffff000000ul) | end << 24;
        return cap;
}
static inline unsigned long cap_channels_get_free(cap_t cap)
{
        return (cap.word0 >> 8) & 0xfffful;
}
static inline cap_t cap_channels_set_free(cap_t cap, unsigned long free)
{
        cap.word0 = (cap.word0 & ~0xffff00ul) | free << 8;
        return cap;
}
static inline cap_t cap_mk_receiver(unsigned long channel, unsigned long grant)
{
        cap_t cap;
        cap.word0 = (unsigned long)CAP_TYPE_RECEIVER;
        cap.word1 = 0;
        cap.word0 |= grant << 8;
        cap.word0 |= channel << 16;
        return cap;
}
static inline unsigned long cap_receiver_get_channel(cap_t cap)
{
        return (cap.word0 >> 16) & 0xfffful;
}
static inline cap_t cap_receiver_set_channel(cap_t cap, unsigned long channel)
{
        cap.word0 = (cap.word0 & ~0xffff0000ul) | channel << 16;
        return cap;
}
static inline unsigned long cap_receiver_get_grant(cap_t cap)
{
        return (cap.word0 >> 8) & 0xfful;
}
static inline cap_t cap_receiver_set_grant(cap_t cap, unsigned long grant)
{
        cap.word0 = (cap.word0 & ~0xff00ul) | grant << 8;
        return cap;
}
static inline cap_t cap_mk_sender(unsigned long channel, unsigned long grant)
{
        cap_t cap;
        cap.word0 = (unsigned long)CAP_TYPE_SENDER;
        cap.word1 = 0;
        cap.word0 |= grant << 8;
        cap.word0 |= channel << 16;
        return cap;
}
static inline unsigned long cap_sender_get_channel(cap_t cap)
{
        return (cap.word0 >> 16) & 0xfffful;
}
static inline cap_t cap_sender_set_channel(cap_t cap, unsigned long channel)
{
        cap.word0 = (cap.word0 & ~0xffff0000ul) | channel << 16;
        return cap;
}
static inline unsigned long cap_sender_get_grant(cap_t cap)
{
        return (cap.word0 >> 8) & 0xfful;
}
static inline cap_t cap_sender_set_grant(cap_t cap, unsigned long grant)
{
        cap.word0 = (cap.word0 & ~0xff00ul) | grant << 8;
        return cap;
}
static inline cap_t cap_mk_server(unsigned long channel, unsigned long grant)
{
        cap_t cap;
        cap.word0 = (unsigned long)CAP_TYPE_SERVER;
        cap.word1 = 0;
        cap.word0 |= grant << 8;
        cap.word0 |= channel << 16;
        return cap;
}
static inline unsigned long cap_server_get_channel(cap_t cap)
{
        return (cap.word0 >> 16) & 0xfffful;
}
static inline cap_t cap_server_set_channel(cap_t cap, unsigned long channel)
{
        cap.word0 = (cap.word0 & ~0xffff0000ul) | channel << 16;
        return cap;
}
static inline unsigned long cap_server_get_grant(cap_t cap)
{
        return (cap.word0 >> 8) & 0xfful;
}
static inline cap_t cap_server_set_grant(cap_t cap, unsigned long grant)
{
        cap.word0 = (cap.word0 & ~0xff00ul) | grant << 8;
        return cap;
}
static inline cap_t cap_mk_client(unsigned long channel, unsigned long grant)
{
        cap_t cap;
        cap.word0 = (unsigned long)CAP_TYPE_CLIENT;
        cap.word1 = 0;
        cap.word0 |= grant << 8;
        cap.word0 |= channel << 16;
        return cap;
}
static inline unsigned long cap_client_get_channel(cap_t cap)
{
        return (cap.word0 >> 16) & 0xfffful;
}
static inline cap_t cap_client_set_channel(cap_t cap, unsigned long channel)
{
        cap.word0 = (cap.word0 & ~0xffff0000ul) | channel << 16;
        return cap;
}
static inline unsigned long cap_client_get_grant(cap_t cap)
{
        return (cap.word0 >> 8) & 0xfful;
}
static inline cap_t cap_client_set_grant(cap_t cap, unsigned long grant)
{
        cap.word0 = (cap.word0 & ~0xff00ul) | grant << 8;
        return cap;
}
static inline cap_t cap_mk_supervisor(unsigned long begin, unsigned long end, unsigned long free)
{
        cap_t cap;
        cap.word0 = (unsigned long)CAP_TYPE_SUPERVISOR;
        cap.word1 = 0;
        cap.word0 |= free << 8;
        cap.word0 |= end << 16;
        cap.word0 |= begin << 24;
        return cap;
}
static inline unsigned long cap_supervisor_get_begin(cap_t cap)
{
        return (cap.word0 >> 24) & 0xfful;
}
static inline cap_t cap_supervisor_set_begin(cap_t cap, unsigned long begin)
{
        cap.word0 = (cap.word0 & ~0xff000000ul) | begin << 24;
        return cap;
}
static inline unsigned long cap_supervisor_get_end(cap_t cap)
{
        return (cap.word0 >> 16) & 0xfful;
}
static inline cap_t cap_supervisor_set_end(cap_t cap, unsigned long end)
{
        cap.word0 = (cap.word0 & ~0xff0000ul) | end << 16;
        return cap;
}
static inline unsigned long cap_supervisor_get_free(cap_t cap)
{
        return (cap.word0 >> 8) & 0xfful;
}
static inline cap_t cap_supervisor_set_free(cap_t cap, unsigned long free)
{
        cap.word0 = (cap.word0 & ~0xff00ul) | free << 8;
        return cap;
}
static inline int cap_is_revokable(cap_t cap)
{
        return cap_is_type(cap, CAP_TYPE_MEMORY) && cap_is_type(cap, CAP_TYPE_TIME) &&
               cap_is_type(cap, CAP_TYPE_CHANNELS) && cap_is_type(cap, CAP_TYPE_RECEIVER) &&
               cap_is_type(cap, CAP_TYPE_SENDER) && cap_is_type(cap, CAP_TYPE_SERVER) &&
               cap_is_type(cap, CAP_TYPE_CLIENT) && cap_is_type(cap, CAP_TYPE_SUPERVISOR);
}
static inline int cap_is_child(cap_t p, cap_t c)
{
        if (cap_is_type(p, CAP_TYPE_MEMORY) && cap_is_type(c, CAP_TYPE_MEMORY))
                return (cap_memory_get_begin(p) <= cap_memory_get_begin(c)) &&
                       (cap_memory_get_end(c) <= cap_memory_get_free(p)) &&
                       ((cap_memory_get_rwx(c) & cap_memory_get_rwx(p)) == cap_memory_get_rwx(c));
        if (cap_is_type(p, CAP_TYPE_MEMORY) && cap_is_type(c, CAP_TYPE_PMP))
                return (cap_memory_get_free(p) <= pmp_napot_begin(cap_pmp_get_addr(c))) &&
                       (pmp_napot_end(cap_pmp_get_addr(c)) <= cap_memory_get_end(p)) && (cap_memory_get_pmp(p) == 1) &&
                       ((cap_pmp_get_rwx(c) & cap_memory_get_rwx(p)) == cap_pmp_get_rwx(c));
        if (cap_is_type(p, CAP_TYPE_TIME) && cap_is_type(c, CAP_TYPE_TIME))
                return (cap_time_get_begin(p) <= cap_time_get_begin(c)) &&
                       (cap_time_get_end(c) <= cap_time_get_free(p)) &&
                       (cap_time_get_hartid(p) == cap_time_get_hartid(c));
        if (cap_is_type(p, CAP_TYPE_CHANNELS) && cap_is_type(c, CAP_TYPE_CHANNELS))
                return (cap_channels_get_begin(p) <= cap_channels_get_begin(c)) &&
                       (cap_channels_get_end(c) <= cap_channels_get_free(p));
        if (cap_is_type(p, CAP_TYPE_CHANNELS) && cap_is_type(c, CAP_TYPE_RECEIVER))
                return (cap_channels_get_begin(p) <= cap_receiver_get_channel(c)) &&
                       (cap_receiver_get_channel(c) < cap_channels_get_free(p));
        if (cap_is_type(p, CAP_TYPE_CHANNELS) && cap_is_type(c, CAP_TYPE_SENDER))
                return (cap_channels_get_begin(p) <= cap_sender_get_channel(c)) &&
                       (cap_sender_get_channel(c) < cap_channels_get_free(p));
        if (cap_is_type(p, CAP_TYPE_CHANNELS) && cap_is_type(c, CAP_TYPE_SERVER))
                return (cap_channels_get_begin(p) <= cap_server_get_channel(c)) &&
                       (cap_server_get_channel(c) < cap_channels_get_free(p));
        if (cap_is_type(p, CAP_TYPE_CHANNELS) && cap_is_type(c, CAP_TYPE_CLIENT))
                return (cap_channels_get_begin(p) <= cap_client_get_channel(c)) &&
                       (cap_client_get_channel(c) < cap_channels_get_free(p));
        if (cap_is_type(p, CAP_TYPE_RECEIVER) && cap_is_type(c, CAP_TYPE_SENDER))
                return (cap_receiver_get_channel(p) == cap_sender_get_channel(c));
        if (cap_is_type(p, CAP_TYPE_SERVER) && cap_is_type(c, CAP_TYPE_CLIENT))
                return (cap_server_get_channel(p) == cap_client_get_channel(c));
        if (cap_is_type(p, CAP_TYPE_SUPERVISOR) && cap_is_type(c, CAP_TYPE_SUPERVISOR))
                return (cap_supervisor_get_begin(p) <= cap_supervisor_get_begin(c)) &&
                       (cap_supervisor_get_end(c) <= cap_supervisor_get_free(p));
        return 0;
}
static inline int cap_can_derive(cap_t p, cap_t c)
{
        if (cap_is_type(p, CAP_TYPE_MEMORY) && cap_is_type(c, CAP_TYPE_MEMORY))
                return (cap_memory_get_pmp(p) == 0) && (cap_memory_get_pmp(c) == 0) &&
                       (cap_memory_get_free(p) == cap_memory_get_begin(c)) &&
                       (cap_memory_get_end(c) <= cap_memory_get_end(p)) &&
                       ((cap_memory_get_rwx(c) & cap_memory_get_rwx(p)) == cap_memory_get_rwx(c)) &&
                       (cap_memory_get_free(c) == cap_memory_get_begin(c)) &&
                       (cap_memory_get_begin(c) < cap_memory_get_end(c));
        if (cap_is_type(p, CAP_TYPE_MEMORY) && cap_is_type(c, CAP_TYPE_PMP))
                return (cap_memory_get_free(p) <= pmp_napot_begin(cap_pmp_get_addr(c))) &&
                       (pmp_napot_end(cap_pmp_get_addr(c)) <= cap_memory_get_end(p)) &&
                       ((cap_pmp_get_rwx(c) & cap_memory_get_rwx(p)) == cap_pmp_get_rwx(c));
        if (cap_is_type(p, CAP_TYPE_TIME) && cap_is_type(c, CAP_TYPE_TIME))
                return (cap_time_get_hartid(p) == cap_time_get_hartid(c)) &&
                       (cap_time_get_free(p) == cap_time_get_begin(c)) &&
                       (cap_time_get_end(c) <= cap_time_get_end(p)) &&
                       (cap_time_get_free(c) == cap_time_get_begin(c)) && (cap_time_get_begin(c) < cap_time_get_end(c));
        if (cap_is_type(p, CAP_TYPE_CHANNELS) && cap_is_type(c, CAP_TYPE_CHANNELS))
                return (cap_channels_get_free(p) == cap_channels_get_begin(c)) &&
                       (cap_channels_get_end(c) <= cap_channels_get_end(p)) &&
                       (cap_channels_get_free(c) == cap_channels_get_begin(c)) &&
                       (cap_channels_get_begin(c) < cap_channels_get_end(c));
        if (cap_is_type(p, CAP_TYPE_CHANNELS) && cap_is_type(c, CAP_TYPE_RECEIVER))
                return (cap_channels_get_free(p) == cap_receiver_get_channel(c)) &&
                       (cap_receiver_get_channel(c) < cap_channels_get_end(p)) &&
                       (cap_receiver_get_grant(c) == 0 || cap_receiver_get_grant(c) == 1);
        if (cap_is_type(p, CAP_TYPE_RECEIVER) && cap_is_type(c, CAP_TYPE_SENDER))
                return (cap_receiver_get_channel(p) == cap_sender_get_channel(c)) &&
                       (cap_receiver_get_grant(p) == cap_sender_get_grant(c));
        if (cap_is_type(p, CAP_TYPE_CHANNELS) && cap_is_type(c, CAP_TYPE_SERVER))
                return (cap_channels_get_free(p) == cap_server_get_channel(c)) &&
                       (cap_server_get_channel(c) < cap_channels_get_end(p)) &&
                       (cap_server_get_grant(c) == 0 || cap_server_get_grant(c) == 1);
        if (cap_is_type(p, CAP_TYPE_SERVER) && cap_is_type(c, CAP_TYPE_CLIENT))
                return (cap_server_get_channel(p) == cap_client_get_channel(c)) &&
                       (cap_server_get_grant(p) == cap_client_get_grant(c));
        if (cap_is_type(p, CAP_TYPE_SUPERVISOR) && cap_is_type(c, CAP_TYPE_SUPERVISOR))
                return (cap_supervisor_get_free(p) <= cap_supervisor_get_begin(c)) &&
                       (cap_supervisor_get_end(c) <= cap_supervisor_get_end(p)) &&
                       (cap_supervisor_get_free(c) == cap_supervisor_get_begin(c)) &&
                       (cap_supervisor_get_begin(c) < cap_supervisor_get_end(c));
        return 0;
}
# 2 "api/s3k_call.h" 2
# 1 "api/../inc/consts.h" 1

enum s3k_state {
        S3K_STATE_READY,
        S3K_STATE_RUNNING,
        S3K_STATE_WAITING,
        S3K_STATE_RECEIVING,
        S3K_STATE_SUSPENDED,
        S3K_STATE_RUNNING_THEN_SUSPEND,
        S3K_STATE_SUSPENDED_BUSY,
        S3K_STATE_RECEIVING_THEN_SUSPEND
};

enum s3k_code {
        S3K_OK,
        S3K_ERROR,
        S3K_EMPTY,
        S3K_PREEMPTED,
        S3K_INTERRUPTED,
        S3K_OCCUPIED,
        S3K_NO_RECEIVER,
        S3K_ILLEGAL_DERIVATION,
        S3K_SUPERVISEE_BUSY,
        S3K_INVALID_SUPERVISEE,
        S3K_INVALID_CAPABILITY,
        S3K_UNIMPLEMENTED,
};

enum s3k_call {
        S3K_SYSCALL_GET_PID,
        S3K_SYSCALL_GET_REG,
        S3K_SYSCALL_SET_REG,
        S3K_SYSCALL_YIELD,

        S3K_SYSCALL_READ_CAP,
        S3K_SYSCALL_MOVE_CAP,
        S3K_SYSCALL_DELETE_CAP,
        S3K_SYSCALL_REVOKE_CAP,
        S3K_SYSCALL_DERIVE_CAP,

        S3K_SYSCALL_SUP_SUSPEND,
        S3K_SYSCALL_SUP_RESUME,
        S3K_SYSCALL_SUP_GET_STATE,
        S3K_SYSCALL_SUP_GET_REG,
        S3K_SYSCALL_SUP_SET_REG,
        S3K_SYSCALL_SUP_READ_CAP,
        S3K_SYSCALL_SUP_MOVE_CAP,

        NUM_OF_SYSNR
};

typedef enum s3k_state s3k_state_t;
typedef enum s3k_code s3k_code_t;
typedef enum s3k_call s3k_call_t;
# 3 "api/s3k_call.h" 2
static inline unsigned long _S3K_SYSCALL(unsigned long sysnr, unsigned long args[8], unsigned long cnt)
{
        register unsigned long t0 __asm__("t0") = sysnr;
        register unsigned long a0 __asm__("a0") = args[0];
        register unsigned long a1 __asm__("a1") = args[1];
        register unsigned long a2 __asm__("a2") = args[2];
        register unsigned long a3 __asm__("a3") = args[3];
        register unsigned long a4 __asm__("a4") = args[4];
        register unsigned long a5 __asm__("a5") = args[5];
        register unsigned long a6 __asm__("a6") = args[6];
        register unsigned long a7 __asm__("a7") = args[7];
        switch (cnt) {
        case 0:
                __asm__ volatile("ecall"
                                 : "+r"(t0), "=r"(a0), "=r"(a1), "=r"(a2), "=r"(a3), "=r"(a4), "=r"(a5), "=r"(a6),
                                   "=r"(a7));
                break;
        case 1:
                __asm__ volatile("ecall"
                                 : "+r"(t0), "+r"(a0), "=r"(a1), "=r"(a2), "=r"(a3), "=r"(a4), "=r"(a5), "=r"(a6),
                                   "=r"(a7));
                break;
        case 2:
                __asm__ volatile("ecall"
                                 : "+r"(t0), "+r"(a0), "+r"(a1), "=r"(a2), "=r"(a3), "=r"(a4), "=r"(a5), "=r"(a6),
                                   "=r"(a7));
                break;
        case 3:
                __asm__ volatile("ecall"
                                 : "+r"(t0), "+r"(a0), "+r"(a1), "+r"(a2), "=r"(a3), "=r"(a4), "=r"(a5), "=r"(a6),
                                   "=r"(a7));
                break;
        case 4:
                __asm__ volatile("ecall"
                                 : "+r"(t0), "+r"(a0), "+r"(a1), "+r"(a2), "+r"(a3), "=r"(a4), "=r"(a5), "=r"(a6),
                                   "=r"(a7));
                break;
        case 5:
                __asm__ volatile("ecall"
                                 : "+r"(t0), "+r"(a0), "+r"(a1), "+r"(a2), "+r"(a3), "+r"(a4), "=r"(a5), "=r"(a6),
                                   "=r"(a7));
                break;
        case 6:
                __asm__ volatile("ecall"
                                 : "+r"(t0), "+r"(a0), "+r"(a1), "+r"(a2), "+r"(a3), "+r"(a4), "+r"(a5), "=r"(a6),
                                   "=r"(a7));
                break;
        case 7:
                __asm__ volatile("ecall"
                                 : "+r"(t0), "+r"(a0), "+r"(a1), "+r"(a2), "+r"(a3), "+r"(a4), "+r"(a5), "+r"(a6),
                                   "=r"(a7));
                break;
        case 8:
                __asm__ volatile("ecall"
                                 : "+r"(t0), "+r"(a0), "+r"(a1), "+r"(a2), "+r"(a3), "+r"(a4), "+r"(a5), "+r"(a6),
                                   "+r"(a7));
                break;
        }
        args[0] = a0;
        args[1] = a1;
        args[2] = a2;
        args[3] = a3;
        args[4] = a4;
        args[5] = a5;
        args[6] = a6;
        args[7] = a7;
        return t0;
}

static inline unsigned long s3k_get_pid(void)
{
        unsigned long args[8];
        _S3K_SYSCALL(S3K_SYSCALL_GET_PID, args, 0);
        return args[0];
}

static inline unsigned long s3k_get_reg(unsigned long reg)
{
        unsigned long args[8] = {reg};
        if (_S3K_SYSCALL(S3K_SYSCALL_GET_REG, args, 1) == 0)
                return args[0];
        return -1;
}

static inline unsigned long s3k_set_reg(unsigned long reg, unsigned long val)
{
        unsigned long args[8] = {reg, val};
        return _S3K_SYSCALL(S3K_SYSCALL_GET_REG, args, 2);
}

static inline unsigned long s3k_yield(void)
{
        unsigned long args[8] = {};
        return _S3K_SYSCALL(S3K_SYSCALL_YIELD, args, 0);
}

unsigned long s3k_read_cap(unsigned long i, cap_t* c)
{
        unsigned long args[8] = {i};
        unsigned long code = _S3K_SYSCALL(S3K_SYSCALL_READ_CAP, args, 1);
        if (!code)
                *c = (cap_t){args[0], args[1]};
        return code;
}

unsigned long s3k_move_cap(unsigned long i, unsigned long j)
{
        unsigned long args[8] = {i, j};
        return _S3K_SYSCALL(S3K_SYSCALL_MOVE_CAP, args, 2);
}

unsigned long s3k_delete_cap(unsigned long i)
{
        unsigned long args[8] = {i};
        return _S3K_SYSCALL(S3K_SYSCALL_DELETE_CAP, args, 1);
}

unsigned long s3k_revoke_cap(unsigned long i)
{
        unsigned long args[8] = {i};
        return _S3K_SYSCALL(S3K_SYSCALL_REVOKE_CAP, args, 1);
}

unsigned long s3k_derive_cap(unsigned long i, unsigned long j, cap_t c)
{
        unsigned long args[8] = {i, j, c.word0, c.word1};
        return _S3K_SYSCALL(S3K_SYSCALL_DERIVE_CAP, args, 4);
}

unsigned long s3k_supervisor_suspend(unsigned long i, unsigned long pid)
{
        unsigned long args[8] = {i, pid};
        return _S3K_SYSCALL(S3K_SYSCALL_SUP_SUSPEND, args, 2);
}

unsigned long s3k_supervisor_resume(unsigned long i, unsigned long pid)
{
        unsigned long args[8] = {i, pid};
        return _S3K_SYSCALL(S3K_SYSCALL_SUP_RESUME, args, 2);
}
unsigned long s3k_supervisor_get_state(unsigned long i, unsigned long pid)
{
        unsigned long args[8] = {i, pid};
        if (!_S3K_SYSCALL(S3K_SYSCALL_SUP_RESUME, args, 2))
                return args[0];
        return -1;
}

unsigned long s3k_supervisor_get_reg(unsigned long i, unsigned long pid, unsigned long* reg)
{
        unsigned long args[8] = {i, pid, *reg};
        unsigned long code = _S3K_SYSCALL(S3K_SYSCALL_SUP_SET_REG, args, 3);
        *reg = args[0];
        return code;
}

unsigned long s3k_supervisor_set_reg(unsigned long i, unsigned long pid, unsigned long reg, unsigned long val)
{
        unsigned long args[8] = {i, pid, reg, val};
        return _S3K_SYSCALL(S3K_SYSCALL_SUP_SET_REG, args, 4);
}

unsigned long s3k_supervisor_read_cap(unsigned long i, unsigned long pid, unsigned long cid, cap_t* cap)
{
        unsigned long args[8] = {i, pid};
        unsigned long code = _S3K_SYSCALL(S3K_SYSCALL_SUP_SET_REG, args, 2);
        if (!code) {
                cap->word0 = args[0];
                cap->word1 = args[1];
        }
        return code;
}

unsigned long s3k_supervisor_move_cap(unsigned long i, unsigned long pid, unsigned long take, unsigned long src,
                                      unsigned long dest)
{
        unsigned long args[8] = {i, pid, take, src, dest};
        return _S3K_SYSCALL(S3K_SYSCALL_SUP_MOVE_CAP, args, 5);
}
