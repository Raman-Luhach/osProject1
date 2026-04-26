#include <sys/time.h>
#include <unistd.h>
#include "../timer/timer.h"

long timer_get_ms(void)
{
    struct timeval tv;

    gettimeofday(&tv, (void *)0);
    return (long)tv.tv_sec * 1000 + (long)tv.tv_usec / 1000;
}

long timer_elapsed(long start_ms)
{
    return timer_get_ms() - start_ms;
}

int timer_has_expired(long start_ms, long duration_ms)
{
    if (timer_elapsed(start_ms) >= duration_ms)
        return 1;
    return 0;
}

void timer_sleep_us(int microseconds)
{
    if (microseconds > 0)
        usleep(microseconds);
}
