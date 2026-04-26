#ifndef CUSTOM_TIMER_H
#define CUSTOM_TIMER_H

/* Returns current time in milliseconds (monotonic clock) */
long timer_get_ms(void);

/* Returns milliseconds elapsed since start_ms */
long timer_elapsed(long start_ms);

/* Returns 1 if duration_ms has passed since start_ms */
int timer_has_expired(long start_ms, long duration_ms);

/* Sleeps for the given number of microseconds (wraps usleep) */
void timer_sleep_us(int microseconds);

#endif
