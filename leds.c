#include "leds.h"
#include "asserts.h"
#include <avr/interrupt.h>


#include "led-sequences.inc"


const uint8_t *const pwmSequences[] PROGMEM = {
	pwm0, pwm1, pwm2, pwm3, pwm4,
};

ASSERT_COMPILE( sizeof(pwmSequences) / sizeof(uint8_t **)
		== NPWMS);


volatile struct FlashyLEDStatus ledStatuses[NLEDS];


ASSERT_COMPILE( sizeof(ledStatuses) / sizeof(struct FlashyLEDStatus) == NLEDS);


/**
 * Count of the number of LEDs turned on.
 *
 * Needs to be carefully managed.  Only change this with interrupts disabled.
 * Because it is a single byte, it can be read without disabling interrupts.
 */
static volatile uint8_t nLEDsOn;


static uint8_t getPWMBrightness(uint8_t led);


void init_leds(void)
{
	nLEDsOn = 0;
	for (uint8_t i=0; i<NLEDS; i++) {
		ledStatuses[i].pwmSequence = 0;
		ledStatuses[i].pwmOnTime = 0;
		ledStatuses[i].pwmCounter = 0;
	}
}


/**
 * Get the total brightness of the LEDs, expressed as a PWM value.
 *
 * Returns a maximum of 100.
 */
uint8_t getTotalPWMBrightness(void)
{
	uint8_t ret = 0;
	uint8_t sreg;
	uint8_t i;

	sreg = SREG;
	cli();
	for (i=0; i<NLEDS; i++) {
		uint8_t br = getPWMBrightness(i);
		ret += br;
		if (ret >= 100) {
			ret = 100;
			break;
		}
	}

	SREG = sreg;
	return ret;
}

void incLEDsOn(void)
{
	uint8_t sreg;

	sreg = SREG;
	cli();
	nLEDsOn ++;
	SREG = sreg;
}


void decLEDsOn(void)
{
	uint8_t sreg;

	sreg = SREG;
	cli();
	nLEDsOn --;
	SREG = sreg;
}


uint8_t led_is_on(uint8_t led)
{
	uint8_t sreg;
	uint8_t ret;

	sreg = SREG;
	cli();
	if (ledStatuses[led].pwmSequence) {
		ret = 1;
	} else {
		ret = 0;
	}
	SREG = sreg;
	return ret;
}


static uint8_t getPWMBrightness(uint8_t led)
{
	uint8_t sreg;
	uint8_t ret;

	sreg = SREG;
	cli();
	if (ledStatuses[led].pwmSequence) {
		ret = pgm_read_byte_near(ledStatuses[led].pwmSequence);
	} else {
		ret = 0;
	}
	SREG = sreg;
	return ret;
}


static void pin_high(uint8_t pinnumber);
static void pin_low(uint8_t pinnumber);


/**
 * Turn on one LED.
 *
 * @param lednumber the index of the LED to turn on.
 */
void led_on(uint8_t lednumber)
{
#ifdef COMMON_ANODE
	pin_low(lednumber);
#else
#ifdef COMMON_CATHODE
	pin_high(lednumber);
#else
#  error Need to #define COMMON_ANODE or COMMON_CATHODE
#endif
#endif
}


/**
 * Turn on one LED.
 *
 * @param lednumber the index of the LED to turn off.
 */
void led_off(uint8_t lednumber)
{
#ifdef COMMON_ANODE
	pin_high(lednumber);
#else
#ifdef COMMON_CATHODE
	pin_low(lednumber);
#else
#  error Need to #define COMMON_ANODE or COMMON_CATHODE
#endif
#endif
}


static void pin_high(uint8_t pinnumber)
{
	/*
	 * This simple switch code is simpler and faster than code that
	 * indirects through a data structure describing the LED port and bits.
	 * The problem with keeping that information in a data structure is
	 * that it can only contain addresses, and information about AVR IO
	 * locations is lost.  This prevents the compiler from optimising the
	 * |= and &= operations into single sbi and cbi instructions.  In
	 * addition, the data structure code has to do indexing into an array
	 * of structures, resulting in a call to the multiplication routine (to
	 * multiply the index by the structure size), making it even slower.
	 */
	switch (pinnumber) {
	case 0: LED1PORT |= (1 << LED1R); break;
	case 1: LED1PORT |= (1 << LED1G); break;
	case 2: LED1PORT |= (1 << LED1B); break;
	case 3: LED2PORT |= (1 << LED2R); break;
	case 4: LED2PORT |= (1 << LED2G); break;
	case 5: LED2PORT |= (1 << LED2B); break;
	case 6: LED3PORT |= (1 << LED3R); break;
	case 7: LED3PORT |= (1 << LED3G); break;
	case 8: LED3PORT |= (1 << LED3B); break;
	}
}


static void pin_low(uint8_t pinnumber)
{
	switch (pinnumber) {
	case 0: LED1PORT &= ~ (1 << LED1R); break;
	case 1: LED1PORT &= ~ (1 << LED1G); break;
	case 2: LED1PORT &= ~ (1 << LED1B); break;
	case 3: LED2PORT &= ~ (1 << LED2R); break;
	case 4: LED2PORT &= ~ (1 << LED2G); break;
	case 5: LED2PORT &= ~ (1 << LED2B); break;
	case 6: LED3PORT &= ~ (1 << LED3R); break;
	case 7: LED3PORT &= ~ (1 << LED3G); break;
	case 8: LED3PORT &= ~ (1 << LED3B); break;
	}
}
