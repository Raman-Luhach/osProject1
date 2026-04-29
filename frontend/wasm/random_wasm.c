/* WASM-compatible random: uses emscripten's random since /dev/urandom unavailable */
#include <emscripten.h>
#include "../../random/random.h"

static unsigned int rng_state = 12345;

void random_init(void)
{
    rng_state = (unsigned int)(emscripten_random() * 4294967295.0);
    if (rng_state == 0)
        rng_state = 12345;
}

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
