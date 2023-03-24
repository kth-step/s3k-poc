#pragma once
#include <stdint.h>

// Generate round keys
void aes128_keyexpansion(const uint32_t key[4], uint32_t round_key[44]);

// Encryption
void aes128_enc(const uint32_t key[4], uint8_t buf[16]);
// Decryption
void aes128_dec(const uint32_t key[4], uint8_t buf[16]);
// Encryption provided round keys
void aes128_enc_rk(const uint32_t round_key[44], uint8_t buf[16]);
// Decryption provided round keys
void aes128_dec_rk(const uint32_t round_key[44], uint8_t buf[16]);
