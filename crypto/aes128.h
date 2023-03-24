#pragma once
#include <stdint.h>

void aes128_enc(const uint32_t key[4], uint8_t buf[16]);
void aes128_dec(const uint32_t key[4], uint8_t buf[16]);
