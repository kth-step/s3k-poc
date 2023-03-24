#include "altio.h"
#include "s3k.h"
#include "aes128.h"

extern void keyexpansion(uint32_t key[44]);
extern void subbytes(uint8_t[16]);
extern void shiftrows(uint8_t[16]);
extern void shiftrows_inv(uint8_t[16]);
extern void mixcolumns(uint8_t[16]);

uint32_t key[4] = {
	0x16157e2b,
	0xa6d2ae28,
	0x8815f7ab,
	0x3c4fcf09
};
uint32_t round_keys[44];

uint8_t buf[16] = {
	0x32, 0x43, 0xf6, 0xa8,
	0x88, 0x5a, 0x30, 0x8d,
	0x31, 0x31, 0x98, 0xa2,
	0xe0, 0x37, 0x07, 0x34
};

void setup(void)
{
	alt_puts("crypto setup");
	aes128_keyexpansion(key, round_keys);
	s3k_yield();
}

void loop(void)
{
	uint64_t start_time, end_time;
	s3k_yield();

	start_time = s3k_gettime();
	aes128_enc_rk(round_keys, buf);
	end_time = s3k_gettime();
	alt_printf("enc_rk 0x%x\n", end_time - start_time);

	start_time = s3k_gettime();
	aes128_dec_rk(round_keys, buf);
	end_time = s3k_gettime();
	alt_printf("dec_rk 0x%x\n", end_time - start_time);
}
