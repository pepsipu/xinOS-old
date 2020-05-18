#include <stdint.h>
#include <stddef.h>

void *memset(void *dest, int c, size_t n)
{
    uint8_t *ptr = (uint8_t *)dest;
    while (n-- > 0)
        *ptr++ = c;
    return dest;
}

void *memcpy(void *dest, const void *src, size_t n)
{
    uint8_t *d = dest;
    const uint8_t *s = src;
    while (n--)
        *d++ = *s++;
    return dest;
}