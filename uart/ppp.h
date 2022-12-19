/**
 * Simple Point-to-Point Protocol Variant
 *
 * LICENCE: MIT
 * AUTHOR: Henrik Karlsson
 */

/**
 * Send data in buf.
 * Returns number of bytes sent.
 */
int ppp_send(const char* restrict buf, int length);

/**
 * Receive data in buf.
 * Returns number of bytes written to buf when successful, otherwise -1.
 */
int ppp_recv(char* restrict buf, int length);
