

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "cpu-speed.h"
#include <util/delay.h>
#include "leds.h"
#include "timer.h"
#include "toggle-pin.h"


static void setup_ports(void);
static void sleep(void);
static void sleep_for_ticks(uint8_t ticks);
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
		static uint8_t counter = 0;

		for (i=0; i< NLEDS; i++) {
			volatile struct FlashyLEDStatus *fls;
			const uint8_t *pwmsequence;

			//TOGGLE_ON();

			fls = ledStatuses + i;
			do {
				cli();
				pwmsequence = fls->pwmSequence;
				sei();
				if (! pwmsequence) {
					break;
				} else {
					sleep_for_ticks(2);
				}
			} while (1);
			pwmsequence = (const uint8_t *)
				pgm_read_word_near(pwmSequences + counter);
			cli();
			fls->pwmSequence = pwmsequence;
			fls->pwmOnTime = pgm_read_byte_near(pwmsequence);
			fls->pwmCounter = 0;
			sei();

			//TOGGLE_OFF();

		}
		counter ++;
		if (counter >= NPWMS) {
			counter = 0;
		}
		sleep_for_ticks(20);
	} while (1);
}


static void sleep_for_ticks(uint8_t ticks)
{
	timeoutCounter = ticks;
	while (timeoutCounter) {
		sleep();
	}
}


static void sleep(void)
{
	uint8_t mcucr;

	/* Idle sleep mode.  Any interrupt can wake us. */
	mcucr = MCUCR;
	mcucr &= ~ ((1 << SM1) | (1 << SM0));
	MCUCR = mcucr;

	MCUCR |= (1 << SE);

	/* Don't separate the following two assembly instructions.  See Atmel's
	   NOTE03. */
	__asm__ __volatile__ ("sei" "\n\t" :: );
	__asm__ __volatile__ ("sleep" "\n\t" :: );

	MCUCR &= ~(1 << SE);
}


static void setup_ports(void)
{
	/* We want to do this as fast as possible to prevent LEDs flashing
	   unnecessarily, so do it with interrupts off. */
	cli();

	LED1DDR |= (1 << LED1R);
	LED1DDR |= (1 << LED1G);
	LED1DDR |= (1 << LED1B);

	LED2DDR |= (1 << LED2R);
	LED2DDR |= (1 << LED2G);
	LED2DDR |= (1 << LED2B);

	LED3DDR |= (1 << LED3R);
	LED3DDR |= (1 << LED3G);
	LED3DDR |= (1 << LED3B);

#ifdef COMMON_ANODE
#  define S(P,B) P |= (1 << B)
#else
#ifdef COMMON_CATHODE
#  define S(P,B) P &= ~ (1 << B)
#else
#  error Need to #define COMMON_ANODE or COMMON_CATHODE
#endif
#endif

	S(LED1PORT, LED1R);
	S(LED1PORT, LED1G);
	S(LED1PORT, LED1B);

	S(LED2PORT, LED2R);
	S(LED2PORT, LED2G);
	S(LED2PORT, LED2B);

	S(LED3PORT, LED3R);
	S(LED3PORT, LED3G);
	S(LED3PORT, LED3B);

	sei();
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
