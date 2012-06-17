

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "cpu-speed.h"
#include <util/delay.h>
#include "leds.h"
#include "timer.h"
#include "toggle-pin.h"
#include <stdlib.h>


static void setup_ports(void);
static void sleep(void);
static void turn_on_a_random_led(void);
static void turn_on_a_group_of_leds(uint8_t n);
static void turn_on_three_same_colour_leds(void);
static void turn_on_rgb_leds(void);
static void set_pwmsequence(volatile struct FlashyLEDStatus *fls,
			    const uint8_t *pwmsequence);
static void sleep_for_ticks(uint8_t ticks);
static uint8_t randbyte(uint8_t limit);
#ifdef STARTUP_REASON
static void startup_reason(void);
#endif

int main(void)
{
	TOGGLE_BEGIN();

#ifdef STARTUP_REASON
	startup_reason();
#endif

	setup_ports();
	init_leds();
	init_timer();

	volatile struct FlashyLEDStatus *fls = ledStatuses;
	const uint8_t *pwmsequence = (const uint8_t *)
		pgm_read_word_near(pwmSequences);
	for (uint8_t i=0; i<NLEDS; i++) {
		set_pwmsequence(fls, pwmsequence);
		// We have to wait for the sequence to get started...
		while (getTotalPWMBrightness() < 4) {
			sleep_for_ticks(2);
		}
		// ... then we wait for it to almost stop.
		while (getTotalPWMBrightness() > 3) {
			sleep_for_ticks(1);
		}
		fls++;
	}

	for (;;) {
		if (getNLEDsOn() < 3 || getTotalPWMBrightness() < 10) {
			uint8_t index = randbyte(250);
			if (index >= 220) {
				turn_on_rgb_leds();
			} else if (index > 200) {
				turn_on_a_group_of_leds(5);
			} else if (index > 150) {
				turn_on_a_group_of_leds(4);
			} else if (index > 100) {
				turn_on_a_group_of_leds(3);
			} else if (index > 90) {
				turn_on_three_same_colour_leds();
			} else {
				turn_on_a_random_led();
			}
		}
		sleep_for_ticks(1);
	}
}


static void turn_on_a_random_led(void)
{
	uint8_t lednumber;
	uint8_t pwmnumber;
	volatile struct FlashyLEDStatus *fls;
	const uint8_t *pwmsequence;

	do {
		lednumber = randbyte(NLEDS);
	} while (led_is_on(lednumber));
	fls = ledStatuses + lednumber;
	pwmnumber = randbyte(NPWMS);
	pwmsequence = (const uint8_t *)
		pgm_read_word_near(pwmSequences + pwmnumber);
	set_pwmsequence(fls, pwmsequence);
}


static void turn_on_a_group_of_leds(uint8_t n)
{
	uint8_t base;
	uint8_t pwmnumber;
	volatile struct FlashyLEDStatus *fls;
	const uint8_t *pwmsequence;
	uint8_t i;

	base = randbyte(NLEDS);
	pwmnumber = randbyte(NPWMS);
	pwmsequence = (const uint8_t *)
		pgm_read_word_near(pwmSequences + pwmnumber);
	/* Do this with interrupts off so we can guarantee that the LEDs start
	   the PWM sequence at the same point. */
	cli();
	for (i=0; i<n; i++) {
		fls = ledStatuses + base;
		set_pwmsequence(fls, pwmsequence);
		base++;
		if (base >= NLEDS) {
			base = 0;
		}
	}
	sei();
}


static void turn_on_three_same_colour_leds(void)
{
	uint8_t colour;		/* R=0, G=1, B=2 */
	uint8_t pwmnumber;
	volatile struct FlashyLEDStatus *fls;
	const uint8_t *pwmsequence;

	colour = randbyte(3);
	fls = ledStatuses + colour;
	pwmnumber = randbyte(NPWMS);
	pwmsequence = (const uint8_t *)
		pgm_read_word_near(pwmSequences + pwmnumber);
	/* Do this with interrupts off so we can guarantee that the LEDs start
	   the PWM sequence at the same point. */
	cli();
	set_pwmsequence(fls, pwmsequence);
	fls += 3;
	set_pwmsequence(fls, pwmsequence);
	fls += 3;
	set_pwmsequence(fls, pwmsequence);
	sei();
}


static void turn_on_rgb_leds(void)
{
	uint8_t pwmnumber;
	const uint8_t *pwmsequence;
	uint8_t start;
	volatile struct FlashyLEDStatus *fls_r;
	volatile struct FlashyLEDStatus *fls_g;
	volatile struct FlashyLEDStatus *fls_b;

	pwmnumber = randbyte(NPWMS);
	pwmsequence = (const uint8_t *)
		pgm_read_word_near(pwmSequences + pwmnumber);
	start = randbyte(3);
	switch (start) {
	case 0:
		fls_r = ledStatuses;
		fls_g = ledStatuses + 4;
		fls_b = ledStatuses + 8;
		break;
	case 1:
		fls_r = ledStatuses + 1;
		fls_g = ledStatuses + 5;
		fls_b = ledStatuses + 6;
		break;
	default:
		fls_r = ledStatuses + 2;
		fls_g = ledStatuses + 3;
		fls_b = ledStatuses + 7;
		break;
	}

	cli();
	set_pwmsequence(fls_r, pwmsequence);
	set_pwmsequence(fls_g, pwmsequence);
	set_pwmsequence(fls_b, pwmsequence);
	sei();
}


static void set_pwmsequence(volatile struct FlashyLEDStatus *fls,
			    const uint8_t *pwmsequence)
{
	uint8_t sreg;

	sreg = SREG;
	cli();
	if ((! fls->pwmSequence) && pwmsequence) {
		incLEDsOn();
	}
	fls->pwmSequence = pwmsequence;
	fls->pwmOnTime = pgm_read_byte_near(pwmsequence);
	fls->pwmCounter = 0;
	SREG = sreg;
}


/**
 * Return a random byte in the interval 0..limit-1.
 *
 * We use the C library's random() to give us a 32 bit random number, and then
 * feed those four bytes back as required.  This way we call random() only each
 * fourth time a byte is required.
 *
 * This is important because random() takes a fairly long time on this CPU.
 * The program had been observed to take up to 37 milliseconds to go back to
 * sleep after an interrupt (before this change), which meant that up to 50
 * interrupts were being handled in that time, before the setup for the next
 * sequence was finished.  After this change, the longest period observed
 * between interrupt and sleep was around 10 milliseconds.
 */
static uint8_t randbyte(uint8_t limit)
{
	static uint8_t counter=5;
	static uint32_t r;
	uint8_t *retp;
	uint8_t ret;

	if (counter >= 4) {
		r = random();
		counter = 0;
	}
	retp = (uint8_t *)(&r);
	ret = retp[counter++];
	if (limit)
		ret %= limit;
	return ret;
}


static void sleep_for_ticks(uint8_t ticks)
{
	// Turn off interrupts here to avoid race conditions.
	timeoutCounter = ticks;
	while (1) {
		if (timeoutCounter) {
			sleep();
		} else {
			return;
		}
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

	/* Turn off the toggle pin so we know the CPU is sleeping. */
	TOGGLE_OFF();

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
