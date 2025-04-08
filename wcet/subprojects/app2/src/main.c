#include "s3k.h"
#include "serio.h"

uint64_t csrr_cycle()
{
	uint64_t tmp;
	__asm__ volatile("rdcycle %0" : "=r"(tmp));
	return tmp;
}

int main(void)
{
	int pid = s3k_get_pid();
	serio_printf("hello from proc %D\n", pid);
	s3k_sleep(0);
	s3k_msg_t msg;
	msg.send_cap = true;
	msg.cap_idx = 3;
	uint64_t start, end;
	while (1) {
		__asm__ volatile(".word 0xb");
		s3k_reply_t reply = s3k_sock_sendrecv(4, &msg);
		end = csrr_cycle();
		if (reply.err) {
			serio_printf("2: Error(%d)\n", reply.err);
		} else {
			serio_printf("2: SUCCESS, %D\n", end);
			s3k_sleep(0);
		}
	}
	return 0;
}
