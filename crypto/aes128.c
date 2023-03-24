#include "aes128.h"

#include <stdint.h>

#include "aes128_tables.h"

static uint32_t subword(uint32_t word) {
	return  (uint32_t)sbox[word & 0xFF] 
		| ((uint32_t)sbox[(word >> 8) & 0xFF] << 8)
		| ((uint32_t)sbox[(word >> 16) & 0xFF] << 16)
		| ((uint32_t)sbox[(word >> 24) & 0xFF] << 24);
}

static uint32_t rotword(uint32_t word) {
	return (word >> 8) | (word << 24);
}

static void keyexpansion(const uint32_t key[4], uint32_t round_keys[44])
{

	for (int i = 0; i < 4; ++i)
		round_keys[i] = key[i];
	for (int i = 4; i < 44; i+=4)
	{
		round_keys[i] = round_keys[i-4] ^ subword(rotword(round_keys[i - 1])) ^ rc[i/4 - 1];
		round_keys[i+1] = round_keys[i-3] ^ round_keys[i];
		round_keys[i+2] = round_keys[i-2] ^ round_keys[i+1];
		round_keys[i+3] = round_keys[i-1] ^ round_keys[i+2];
	}
}

static void subshiftrows(uint8_t buf[16])
{
	uint8_t tmp;

	// 1st row
	buf[0] = sbox[buf[0]];
	buf[4] = sbox[buf[4]];
	buf[8] = sbox[buf[8]];
	buf[12] = sbox[buf[12]];

	// 2nd row
	tmp = sbox[buf[1]];
	buf[1] = sbox[buf[5]];
	buf[5] = sbox[buf[9]];
	buf[9] = sbox[buf[13]];
	buf[13] = tmp;

	// 3rd row
	tmp = sbox[buf[2]];
	buf[2] = sbox[buf[10]];
	buf[10] = tmp;
	tmp = sbox[buf[6]];
	buf[6] = sbox[buf[14]];
	buf[14] = tmp;

	// 4th row
	tmp = sbox[buf[15]];
	buf[15] = sbox[buf[11]];
	buf[11] = sbox[buf[7]];
	buf[7] = sbox[buf[3]];
	buf[3] = tmp;
}

static void subshiftrows_inv(uint8_t buf[16])
{
	uint8_t tmp;

	// 1st row
	buf[0] =  sbox_inv[buf[0]];
	buf[4] =  sbox_inv[buf[4]];
	buf[8] =  sbox_inv[buf[8]];
	buf[12] = sbox_inv[buf[12]];

	// 2nd row
	tmp = sbox_inv[buf[13]];
	buf[13] = sbox_inv[buf[9]];
	buf[9] = sbox_inv[buf[5]];
	buf[5] = sbox_inv[buf[1]];
	buf[1] = tmp;

	// 3rd row
	tmp = sbox_inv[buf[10]];
	buf[10] = sbox_inv[buf[2]];
	buf[2] = tmp;
	tmp = sbox_inv[buf[14]];
	buf[14] =sbox_inv[buf[6]];
	buf[6] = tmp;

	// 4th row
	tmp =    sbox_inv[buf[11]];
	buf[11] =sbox_inv[buf[15]];
	buf[15] =sbox_inv[buf[3]];
	buf[3] = sbox_inv[buf[7]];
	buf[7] = tmp;
}

static void mixcolumns(uint8_t buf[16])
{
	uint8_t t[4];
	for (uint8_t i = 0; i < 16; i+=4) {
		t[0] = buf[i + 0];
		t[1] = buf[i + 1];
		t[2] = buf[i + 2];
		t[3] = buf[i + 3];
		buf[i + 0] = table_2[t[0]] ^ table_3[t[1]] ^ t[2] ^ t[3];
		buf[i + 1] = table_2[t[1]] ^ table_3[t[2]] ^ t[3] ^ t[0];
		buf[i + 2] = table_2[t[2]] ^ table_3[t[3]] ^ t[0] ^ t[1];
		buf[i + 3] = table_2[t[3]] ^ table_3[t[0]] ^ t[1] ^ t[2];
	}
}

static void mixcolumns_inv(uint8_t buf[16])
{
	uint8_t t[4];
	for (uint8_t i = 0; i < 16; i+=4) {
		t[0] = buf[i + 0];
		t[1] = buf[i + 1];
		t[2] = buf[i + 2];
		t[3] = buf[i + 3];
		buf[i + 0] = table_14[t[0]] ^ table_11[t[1]] ^ table_13[t[2]] ^ table_9[t[3]];
		buf[i + 1] = table_14[t[1]] ^ table_11[t[2]] ^ table_13[t[3]] ^ table_9[t[0]];
		buf[i + 2] = table_14[t[2]] ^ table_11[t[3]] ^ table_13[t[0]] ^ table_9[t[1]];
		buf[i + 3] = table_14[t[3]] ^ table_11[t[0]] ^ table_13[t[1]] ^ table_9[t[2]];
	}
}

static void addroundkey(uint8_t buf[16], uint8_t key[16])
{
	for (int i = 0; i < 16; ++i)
		buf[i] ^= key[i];
}

void aes128_enc(const uint32_t key[4], uint8_t buf[16])
{
	uint32_t round_keys[44];
	keyexpansion(key, round_keys);
	addroundkey(buf, (uint8_t*)round_keys);
	for (int i = 1; i <= 9; ++i) {
		subshiftrows(buf);
		mixcolumns(buf);
		addroundkey(buf, (uint8_t*)&round_keys[i*4]);
	}
	subshiftrows(buf);
	addroundkey(buf, (uint8_t*)&round_keys[40]);
}

void aes128_dec(const uint32_t key[4], uint8_t buf[16])
{
	uint32_t round_keys[44];
	keyexpansion(key, round_keys);
	addroundkey(buf, (uint8_t*)&round_keys[40]);
	subshiftrows_inv(buf);
	for (int i = 9; i >= 1; --i) {
		addroundkey(buf, (uint8_t*)&round_keys[i*4]);
		mixcolumns_inv(buf);
		subshiftrows_inv(buf);
	}
	addroundkey(buf, (uint8_t*)round_keys);
}
