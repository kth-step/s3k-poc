#include "ppp.h"
#include "util.h"

void main(void)
{
        char buf[256];
        ppp_send("uart_ready", 10);
        while (1) {
                int len = ppp_recv(buf, 256);
                if (len > 0)
                        ppp_send(buf, len);
        }
}
