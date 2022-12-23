#include "ppp.h"

#define BUF_SIZE 256

void main(void)
{
        char buf[BUF_SIZE];
        const char *msg = "frame too long";
        ppp_send("uart_ready", 10);
        while (1) {
                int len = ppp_recv(buf, BUF_SIZE);
                if (len > 0)
                        ppp_send(buf, len);
                else
                        ppp_send(msg, 14);
        }
}
