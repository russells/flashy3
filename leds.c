#include "leds.h"
#include "asserts.h"


const uint8_t pwm0[] PROGMEM = {
	1, 2, 3, 4, 5, 8, 10, 12, 14, 17, 19, 17, 14, 12, 10, 8, 5, 4, 3, 2, 1, 0,
};

const uint8_t pwm1[] PROGMEM = {
	1, 2, 5, 10, 15, 10, 5, 2, 1, 0,
};

const uint8_t pwm2[] PROGMEM = {
	1, 2, 5, 10, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 10, 5, 2, 1, 0,
};

const uint8_t pwm3[] PROGMEM = {
	1, 5, 10, 5, 1, 0,
};

const uint8_t pwm4[] PROGMEM = {
	1, 2, 2, 2, 2, 5, 10, 10, 18, 18, 10, 10, 5, 2, 2, 2, 2, 1, 0,
};


const uint8_t *pwmSequences[] PROGMEM = {
	pwm0, pwm1, pwm2, pwm3, pwm4,
};


ASSERT_COMPILE( sizeof(pwmSequences) / sizeof(uint8_t **)
		== NPWMS);


volatile struct FlashyLEDStatus ledStatuses[NLEDS];


ASSERT_COMPILE( sizeof(ledStatuses) / sizeof(struct FlashyLEDStatus) == NLEDS);


void init_leds(void)
{
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
	pin_high(lednumber);
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
	pin_low(lednumber);
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
