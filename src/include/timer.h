#ifndef TIMER_H
#define TIMER_H

#include "types.h"

void timer_init(uint32_t frequency);
uint32_t timer_get_ticks(void);
void timer_sleep(uint32_t ticks);
void timer_sleep_ms(uint32_t ms);

#endif // TIMER_H
