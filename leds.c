#include "leds.h"
#include "asserts.h"


const struct FlashyLED leds[] PROGMEM = {
	/* LED 1 */
	{
		&(LED1PORT),
		&(LED1DDR),
		(1 << LED1R),
		~ (1 << LED1R),
	},
	{
		&(LED1PORT),
		&(LED1DDR),
		(1 << LED1G),
		~ (1 << LED1G),
	},
	{
		&(LED1PORT),
		&(LED1DDR),
		(1 << LED1B),
		~ (1 << LED1B),
	},
	/* LED 2 */
	{
		&(LED2PORT),
		&(LED2DDR),
		(1 << LED2R),
		~ (1 << LED2R),
	},
	{
		&(LED2PORT),
		&(LED2DDR),
		(1 << LED2G),
		~ (1 << LED2G),
	},
	{
		&(LED2PORT),
		&(LED2DDR),
		(1 << LED2B),
		~ (1 << LED2B),
	},
	/* LED 3 */
	{
		&(LED3PORT),
		&(LED3DDR),
		(1 << LED3R),
		~ (1 << LED3R),
	},
	{
		&(LED3PORT),
		&(LED3DDR),
		(1 << LED3G),
		~ (1 << LED3G),
	},
	{
		&(LED3PORT),
		&(LED3DDR),
		(1 << LED3B),
		~ (1 << LED3B),
	},
};

ASSERT_COMPILE( sizeof(leds)/sizeof(struct FlashyLED) == NLEDS );


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
		== NPWMSEQUENCES);


volatile struct FlashyLEDStatus ledStatuses[NLEDS];


ASSERT_COMPILE( sizeof(ledStatuses) / sizeof(struct FlashyLEDStatus) == NLEDS);


void init_leds(void)
{
}
