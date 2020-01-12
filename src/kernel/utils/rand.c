uint32_t rdtsc(){
    uint64_t ret;
    asm volatile ("rdtsc":"=A"(ret));
    return (uint32_t) ret;
}

uint16_t max_rand(int max)
{
    uint64_t random_seed = rdtsc() * 9679 + 21269;
    return (uint16_t) (random_seed / 65536) % (max+1);
}