#ifndef CUSTOM_RANDOM_H
#define CUSTOM_RANDOM_H

/* Seeds the PRNG from /dev/urandom (OS entropy source) */
void random_init(void);

/* Returns a pseudo-random number between 0 and max-1 */
int random_range(int max);

/* Returns a pseudo-random number between min and max (inclusive) */
int random_between(int min, int max);

#endif
