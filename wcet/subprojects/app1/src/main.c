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

	s3k_msg_t msg;
	msg.send_cap = true;
	msg.cap_idx = 3;
	while (1) {
		s3k_reply_t reply = s3k_sock_sendrecv(4, &msg);
		if (reply.err) {
			serio_printf("1: Error %d\n", reply.err);
		}
	}
	return 0;
}
