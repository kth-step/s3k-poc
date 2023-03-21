#pragma once
#include <stdbool.h>
#include <stdint.h>

#include "s3k.h"

void capman_init(void);
union s3k_cap capman_get(uint64_t idx);
bool capman_move(uint64_t src, uint64_t dst);
void capman_update(void);
void capman_dump(void);
void capman_getpmp(uint8_t pmp[8]);
void capman_setpmp(uint8_t pmp[8]);
int capman_derive_mem(uint64_t begin, uint64_t end, uint64_t rwx);
int capman_derive_pmp(uint64_t begin, uint64_t end, uint64_t rwx);
