#include "aes128.h"
#include "altio.h"
#include "s3k.h"

// Encryption key
uint32_t enc_key[4] = {
    0x16157e2b,
    0xa6d2ae28,
    0x8815f7ab,
    0x3c4fcf09,
};

// MAC key
uint32_t mac_key[4] = {
    0x0f8955f2,
    0xebc5973a,
    0x546c28e2,
    0xe111547d,
};

// Encryption round keys
uint32_t enc_rk[44];
// MAC round keys
uint32_t mac_rk[44];

// Initialization vector
uint32_t iv[4];

// Message buffer
uint8_t buf[16] = {0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d,
		   0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34};

void setup(void)
{
	alt_puts("crypto setup");
	aes128_keyexpansion(enc_key, enc_rk);
	aes128_keyexpansion(mac_key, mac_rk);
	s3k_yield();
}

void loop(void)
{
	uint64_t recv_buf[4] = {0xa, 0xb, 0xc, 0xd};
	uint64_t tag;
	uint64_t start_time, end_time;
	s3k_sendrecv(0xa, 0x9, recv_buf, -1ull, -1ull, &tag);
	alt_printf("crypto: tag=%X msg=%X,%X,%X,%X\n", tag, 
			recv_buf[0], recv_buf[1], recv_buf[2], recv_buf[3]);

	start_time = s3k_gettime();
	aes128_enc(enc_rk, buf);
	end_time = s3k_gettime();
	alt_printf("enc_rk 0x%x\n", end_time - start_time);

	start_time = s3k_gettime();
	aes128_dec(enc_rk, buf);
	end_time = s3k_gettime();
	alt_printf("dec_rk 0x%x\n", end_time - start_time);
}
