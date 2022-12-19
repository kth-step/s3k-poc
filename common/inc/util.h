static inline void memcpy(void *restrict dest, const void * restrict src, int len) {
        char *cdest = dest;
        const char *csrc = src;
        for (int i = 0; i < len; i++) {
                cdest[i] = csrc[i];
        }
}

static inline int strlen(const char *s) {
        int len = 0;
        while (s[len] != '\0')
                len++;
        return len;
}
