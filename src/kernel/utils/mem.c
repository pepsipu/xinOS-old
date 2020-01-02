#ifndef MEM
#define MEM

#include <stdint.h>
#include <glob.h>

void *memset(void *s, int c, size_t n) {
    for (size_t i = 0; i < n; i++) {
        ((char *) s)[i] = (uint8_t)c;
    }
    return s;
}

void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *pdest = dest;
    const uint8_t *psrc = src;
    for (size_t i = 0; i < n; i++) {
        ((char *) dest)[i] = psrc[i];
    }
    return dest;
}

#endif