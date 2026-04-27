#include <fcntl.h>
#include <unistd.h>
#include "../random/random.h"

static unsigned int rng_state = 12345;

/* Seed from /dev/urandom - the OS kernel's entropy pool */
void random_init(void)
{
    int fd;
    unsigned int seed;

    fd = open("/dev/urandom", O_RDONLY);
    if (fd >= 0)
    {
        if (read(fd, &seed, sizeof(seed)) == (int)sizeof(seed))
            rng_state = seed;
        close(fd);
    }
}

/* xorshift32 PRNG algorithm */
static unsigned int xorshift(void)
{
    rng_state ^= rng_state << 13;
    rng_state ^= rng_state >> 17;
    rng_state ^= rng_state << 5;
    return rng_state;
}

int random_range(int max)
{
    if (max <= 0)
        return 0;
    return (int)(xorshift() % (unsigned int)max);
}

int random_between(int min, int max)
{
    if (max <= min)
        return min;
    return min + random_range(max - min + 1);
}
