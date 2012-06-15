#ifndef timer_h_INCLUDED
#define timer_h_INCLUDED

#include <stdint.h>

/**
 * Timeout counter for the main loop.
 *
 * If set to a non-zero value, this counter is decremented by the ISR
 * approximately 10 times per second.  We can therefore set a timeout up to
 * about 25.5 seconds.
 *
 * No action is automatically taken when this counter reaches zero.  It's up to
 * the main loop to monitor the value and take action.
 */
extern volatile uint8_t timeoutCounter;

void init_timer(void);

#endif
