#include "ppp.h"

#include <stdbool.h>

#include "uart.h"

#define PPP_ESCAPE '|'
#define PPP_SEQ '}'
#define PPP_COMP ' '

/**
 * Send data in buf.
 * Returns number of bytes sent.
 */
int ppp_send(const char* restrict buf, int length)
{
        uart_putchar(PPP_SEQ);
        for (int i = 0; i < length; i++) {
                switch (buf[i]) {
                case PPP_ESCAPE:
                case PPP_SEQ:
                        /* Escape control character */
                        uart_putchar(PPP_ESCAPE);
                        uart_putchar(buf[i] ^ PPP_COMP);
                        break;
                default:
                        uart_putchar(buf[i]);
                }
        }
        uart_putchar(PPP_SEQ);
        return length;
}

/**
 * Receive data in buf.
 * Returns number of bytes written to buf when successful, otherwise -1.
 */
int ppp_recv(char* restrict buf, int length)
{
        static int i = 0;
        bool escape = false;
        while (1) {
                char c = uart_getchar();
                switch (c) {
                case PPP_ESCAPE: /* Escape character */
                        escape = true;
                        break;
                case PPP_SEQ: /* Restart */
                        if (i > 0) {
                                int tmp = i;
                                i = 0;
                                return tmp;
                        }
                        break;
                default:
                        if (i < length) { /* Save a byte */
                                buf[i++] = escape ? (PPP_COMP ^ c) : c;
                                escape = false;
                        } else { /* Message too long */
                                return -1;
                        }
                }
        }
}
