/* WASM-compatible timer: uses emscripten_get_now() (performance.now() in browser) */
#include <emscripten.h>
#include "../../timer/timer.h"

long timer_get_ms(void)
{
    return (long)emscripten_get_now();
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
    /* No-op in WASM — browser handles frame timing */
    (void)microseconds;
}
