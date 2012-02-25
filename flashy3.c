

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "cpu-speed.h"
#include <util/delay.h>
#include "leds.h"
#include "timer.h"
#include "toggle-pin.h"


static void setup_ports(void);
#ifdef STARTUP_REASON
static void startup_reason(void);
#endif

int main(void)
{
	uint8_t i;
	TOGGLE_BEGIN();

#ifdef STARTUP_REASON
	startup_reason();
#endif

	setup_ports();
	init_leds();
	init_timer();
	do {
		static uint8_t counter = 77;

		for (i=0; i< NLEDS; i++) {
			volatile struct FlashyLEDStatus *fls;
			const uint8_t *pwmsequence;

			TOGGLE_ON();

			counter ++;

			fls = ledStatuses + i;
			pwmsequence = (const uint8_t *)
				pgm_read_word_near(pwmSequences+(counter%NPWMS));
			cli();
			fls->pwmSequence = pwmsequence;
			fls->pwmOnTime = pgm_read_byte_near(pwmsequence);
			fls->pwmCounter = 0;

			TOGGLE_OFF();

			sei();
			_delay_ms(100);
		}
	} while (1);
}


static void setup_ports(void)
{
	LED1DDR |= (1 << LED1R);
	LED1DDR |= (1 << LED1G);
	LED1DDR |= (1 << LED1B);

	LED2DDR |= (1 << LED2R);
	LED2DDR |= (1 << LED2G);
	LED2DDR |= (1 << LED2B);

	LED3DDR |= (1 << LED3R);
	LED3DDR |= (1 << LED3G);
	LED3DDR |= (1 << LED3B);

	LED1PORT &= ~ (1 << LED1R);
	LED1PORT &= ~ (1 << LED1G);
	LED1PORT &= ~ (1 << LED1B);

	LED2PORT &= ~ (1 << LED2R);
	LED2PORT &= ~ (1 << LED2G);
	LED2PORT &= ~ (1 << LED2B);

	LED3PORT &= ~ (1 << LED3R);
	LED3PORT &= ~ (1 << LED3G);
	LED3PORT &= ~ (1 << LED3B);
}


#ifdef STARTUP_REASON
/**
 * Indicate the reason for reset by flashing.
 */
static void startup_reason(void)
{
	uint8_t i;
        uint8_t mcusr;

        mcusr = MCUSR;
        MCUSR = 0;

        /* One long flash first. */
	TOGGLE_ON();
        _delay_ms(500);
	TOGGLE_OFF();
        _delay_ms(500);

        /* Now one or two flashes for each of the MCUSR bits.  The total length
           of the one flash or the two flashes and the pause between them
           should be the same. */
        for (i=0; i<4; i++) {
                if (mcusr & 0x1) {
			TOGGLE_ON();
                        _delay_ms(10);
			TOGGLE_OFF();
                        _delay_ms(180);
			TOGGLE_ON();
                        _delay_ms(10);
			TOGGLE_OFF();
                        _delay_ms(800);
                        /* 10+180+10+800 = 1000 */
                } else {
			TOGGLE_ON();
                        _delay_ms(10);
			TOGGLE_OFF();
                        _delay_ms(990);
                        /* 10+990 = 1000 */
                }
                mcusr >>= 1;
        }

        /* One long flash. */
	TOGGLE_ON();
        _delay_ms(500);
	TOGGLE_OFF();
}
#endif
