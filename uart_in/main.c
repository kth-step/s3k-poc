#include <stdint.h>
#include <stdbool.h>

#include "printf.h"
#include "s3k.h"
#include "uart.h"

#define BUF_LENGTH 256

void setup(void)
{
        uart_puts("uart_in setup\n");
}

static inline bool check_buffer(int i, uint8_t buffer[BUF_LENGTH]) {
        uint8_t chk0 = 0, chk1=0;
        for (int j = 0; j < BUF_LENGTH; j+=2){
                chk0 ^= buffer[(i + j) % BUF_LENGTH];
                chk1 ^= buffer[(i + j+1) % BUF_LENGTH];
        }
        printf("chk0 == %d && chk1 == %d\n", chk0, chk1);
        return chk0 == 0 && chk1 == 0;
}

static inline void clear_buffer(int *i, uint8_t buffer[BUF_LENGTH]) {
        *i = 0;
        for (int j = 0; j < BUF_LENGTH; j++)
                buffer[j] = 0;
}

void loop(void)
{
        static int i = 0;
        static uint8_t buffer[BUF_LENGTH];
        while (true) {
                char c = uart_getchar();
                buffer[i++] = c;
                if (c == '\n') {
                        buffer[i++] = '\0';
                        uart_puts(buffer);
                        i = 0;
                }
        }
}
