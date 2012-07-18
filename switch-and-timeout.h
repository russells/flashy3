#ifndef switch_and_timeout_h_INCLUDED
#define switch_and_timeout_h_INCLUDED

/**
 * Latch the power switch on.
 *
 * The function that turns the switch off is private, since it's called from
 * switch_and_timeout_check().
 */
void switch_latch(void);

/**
 * If the button is pressed again, or we've been on for a long time, turn off.
 *
 * The button is debounced by checking that it reads as released three times in
 * a row before starting to check for button pressed.
 *
 * A long time is something like 15 minutes.
 */
void switch_and_timeout_check(void);

#endif