#include "aes128.h"
#include "altio.h"
#include "s3k.h"

#define MEM_SLICE_CIDX 0
#define MAIN_PMP_CIDX 1
#define UART_PMP_CIDX 2
#define SOCKET_CIDX 3
#define TIME_CIDX 4

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
uint32_t iv[4] = {0, 1, 2, 3};


void setup(void)
{
	alt_puts("crypto setup");
	aes128_keyexpansion(enc_key, enc_rk);
	aes128_keyexpansion(mac_key, mac_rk);
	s3k_yield();
}

void decrypt(uint8_t buf[16], uint8_t checksum[16])
{
	uint8_t mac[16];
	// Decrypt buffer
	aes128_dec(enc_rk, buf);
	// Computer CBC MAC on plaintext
	aes128_cbc_mac(mac_rk, buf, mac, 16);
	// XOR with checksum
	for (int i = 0; i < 16; ++i)
		checksum[i] ^= mac[i];
	// checksum == 0 if authentic
}

void encrypt(uint8_t buf[16], uint8_t checksum[16])
{
	// Compute CBC MAC on plaintext
	aes128_cbc_mac(mac_rk, buf, checksum, 16);
	// Encrypt plaintext
	aes128_enc(enc_rk, buf);
}

void loop(void)
{
	uint64_t tag;
	// Message buffer
	uint64_t buf[4];
	uint64_t buf_cidx = TIME_CIDX;
	s3k_cap_t cap;
	while (1) {
		// Reply and listen for monitor
		s3k_sendrecv_cap(SOCKET_CIDX, buf, buf_cidx, &cap, &tag);
		
		if (tag & 1) {
			// Odd tag => encrypt
			encrypt((uint8_t*)buf, (uint8_t*)&buf[2]);
		} else {
			// Even tag => decrypt
			decrypt((uint8_t*)buf, (uint8_t*)&buf[2]);
		}

	}
	
}
