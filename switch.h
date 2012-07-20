#ifndef switch_h_INCLUDED
#define switch_h_INCLUDED

#include <stdint.h>

/**
 * Latch the power switch on.
 *
 * The function that turns the switch off is private, since it's called from
 * switch_and_timeout_check().
 */
void switch_latch(void);

/**
 * Turn off the power switch.
 *
 * Call this when the flashy needs to turn off for some reason - either it's
 * been on for long enough, or the button has been pressed.
 */
void switch_off(void);

/**
 * If the button is pressed again, or we've been on for a long time, turn off.
 *
 * The button is debounced by checking that it reads as released three times in
 * a row before starting to check for button pressed.
 *
 * A long time is something like 15 minutes.
 */
void switch_and_timeout_check(void);

/**
 * Get sort-of random 10 bit numbers from the button ADC.
 */
uint16_t get_10_bit_button_adc(void);

#endif
