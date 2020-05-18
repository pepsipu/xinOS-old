#include <stdint.h>
#include <stddef.h>

void *memset(void *dest, int c, size_t n)
{
    uint8_t *ptr = (uint8_t *)dest;
    while (n-- > 0)
        *ptr++ = c;
    return dest;
}