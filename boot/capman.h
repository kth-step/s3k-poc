#pragma once
#include <stdbool.h>
#include <stdint.h>

void capman_init(void);
void capman_update(void);
void capman_getpmp(uint8_t pmp[8]);
void capman_setpmp(uint8_t pmp[8]);
int capman_derive_mem(uint64_t begin, uint64_t end, uint64_t rwx);
int capman_derive_pmp(uint64_t begin, uint64_t end, uint64_t rwx);
