#include <stdint.h>

inline void cpuid(uint32_t id, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx)
{
    uint32_t _eax, _ebx, _ecx, _edx;
    asm volatile("cpuid"
                 : "=a"(_eax), "=b"(_ebx), "=c"(_ecx), "=d"(_edx)
                 : "a"(id));
    if (eax)
        *eax = _eax;
    if (ebx)
        *ebx = _ebx;
    if (ecx)
        *ecx = _ecx;
    if (edx)
        *edx = _edx;
}