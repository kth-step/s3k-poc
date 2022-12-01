#include <stdbool.h>
#include <stdint.h>

#include "printf.h"
#include "s3k.h"
#include "uart.h"

#define UART_IN 0
#define UART_OUT 1
#define APP0 2
#define APP1 3

struct payload {
        uint8_t* begin;
        uint8_t* end;
};

extern struct payload payloads[4];
extern uint8_t free_space[8][16384];
extern void payload_load(uint8_t*, uint8_t*, uint8_t*);

void _puts(char* c)
{
        uart_puts(c);
}

void print_cap(cap_t cap)
{
        switch (cap_get_type(cap)) {
        case CAP_TYPE_EMPTY:
                printf("EMPTY{}\n");
                break;
        case CAP_TYPE_MEMORY:
                printf("MEMORY{begin=0x%lx,end=0x%lx,rwx=0x%lx,free=0x%lx}\n", cap_memory_get_begin(cap),
                       cap_memory_get_end(cap), cap_memory_get_rwx(cap), cap_memory_get_free(cap));
                break;
        case CAP_TYPE_PMP:
                printf("PMP{addr=0x%lx,rwx=0x%lx}\n", cap_pmp_get_addr(cap), cap_pmp_get_rwx(cap));
                break;
        case CAP_TYPE_TIME:
                printf("TIME{hartid=%ld,begin=%ld,end=%ld,free=%ld}\n", cap_time_get_hartid(cap),
                       cap_time_get_begin(cap), cap_time_get_end(cap), cap_time_get_free(cap));
                break;
        case CAP_TYPE_CHANNELS:
                printf("CHANNELS{begin=0x%lx,end=0x%lx,free=0x%lx}\n", cap_channels_get_begin(cap),
                       cap_channels_get_end(cap), cap_channels_get_free(cap));
                break;
        case CAP_TYPE_RECEIVER:
                printf("RECEIVER{}\n");
                break;
        case CAP_TYPE_SENDER:
                printf("SENDER{}\n");
                break;
        case CAP_TYPE_SUPERVISOR:
                printf("SUPERVISOR{begin=%ld,end=%ld,free=%ld}\n", cap_supervisor_get_begin(cap),
                       cap_supervisor_get_end(cap), cap_supervisor_get_free(cap));
                break;
        default:
                printf("Unknown\n");
        }
}

void dump_cap(void)
{
        for (int i = 0; i < 20; i++) {
                cap_t cap;
                uint64_t code = s3k_read_cap(i, &cap);
                if (code == S3K_OK && cap.word0) {
                        printf("%2d: ", i);
                        print_cap(cap);
                }
        }
}

void setup(void)
{
        uart_puts("monitor setup\n");
        dump_cap();

        s3k_delete_cap(4);
        s3k_delete_cap(5);
        s3k_delete_cap(6);
        s3k_yield();

        cap_t uart_in_time = cap_mk_time(1, 0, 25, 0);
        cap_t monitor_time = cap_mk_time(1, 25, 50, 25);
        cap_t app0_time = cap_mk_time(1, 50, 75, 50);
        cap_t uart_out_time = cap_mk_time(1, 75, 100, 75);
        dump_cap();
        while (S3K_OK != s3k_derive_cap(3, 16, uart_in_time))
                ;
        while (S3K_OK != s3k_derive_cap(3, 17, monitor_time))
                ;
        while (S3K_OK != s3k_derive_cap(3, 18, app0_time))
                ;
        while (S3K_OK != s3k_derive_cap(3, 19, uart_out_time))
                ;
        dump_cap();
        printf("Give cap 16\n");
        while (S3K_OK != s3k_supervisor_move_cap(2, 1, false, 16, 1))
                ;
        printf("Give cap 19\n");
        while (S3K_OK != s3k_supervisor_move_cap(2, 2, false, 19, 1))
                ;
        printf("Give cap 18\n");
        while (S3K_OK != s3k_supervisor_move_cap(2, 3, false, 18, 1))
                ;

        for (int i = 0; i < 4; i++)
                payload_load(payloads[i].begin, payloads[i].end, free_space[i]);
        s3k_supervisor_set_reg(2, 1, 0, (uint64_t)free_space[UART_IN]);
        s3k_supervisor_set_reg(2, 2, 0, (uint64_t)free_space[UART_OUT]);
        s3k_supervisor_set_reg(2, 3, 0, (uint64_t)free_space[APP0]);
        s3k_supervisor_resume(2, 1);
        s3k_supervisor_resume(2, 2);
        s3k_supervisor_resume(2, 3);
        dump_cap();
        s3k_yield();
}

void loop(void)
{
        uart_puts("monitor loop\n");
        s3k_yield();
}
