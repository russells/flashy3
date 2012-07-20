#include "switch.h"
#include <avr/io.h>


void switch_latch(void)
{
	/* Low output to keep the power turned on. */
	DDRB |= (1 << 7);
	PORTB &= ~ (1 << 7);

	/** Set up the ADC for ADC0. */
	ADMUX = (1 << REFS0) |	/* Ref = 1.1V */
		(0b000 << MUX0); /* Input = ADC0 */
	ADCSRA = (1 << ADEN) |	 /* Enable */
		(0 << ADSC) |	 /* Don't start yet */
		(0 << ADATE) |	 /* No auto trigger */
		(1 << ADIF) |	 /* Clear any pending interrupt */
		(0 << ADIE) |	 /* No interrupts */
		(0b010 << ADPS0); /* CLKio/4 ~= 250kHz */
	ADCSRB = (1 << ADLAR) |	 /* 8 bit results */
		(0b000 << ADTS0); /* Free running, but not auto triggered */
}


void switch_off(void)
{
	PORTB |= (1 << 7);
}


/* Anything more than this and we assume the button is released.  The value is
   about 1/10 of the reference at eight bit resolution, which is about 0.11V
   with the 1.1V reference.  The released button should be at about one diode
   drop below 1.5V, so at about 0.8V, way above 0.11V.  This low level was
   chosen because the boost converter can continue to work down to 0.8V, which
   would put the open button voltage around 0.1V. */
#define BUTTON_RELEASED 25

/* Anything less than this makes us assume the button is pressed.  This
   corresponds to about 50mV, and our button should pull down below that. */
#define BUTTON_PRESSED 12


void switch_and_timeout_check(void)
{
	static uint8_t button_released_counter = 3;
	uint8_t button_value;

	ADCSRA |= (1 << ADSC);
	do {
		/* nothing */
	} while (ADCSRA & (1 << ADSC));
	button_value = ADCH;
	if (button_released_counter) {
		/* Check for button release */
		if (button_value > BUTTON_RELEASED) {
			button_released_counter --;
		} else {
			/* If we're still waiting for button release and we
			   sense that it hasn't been released, start the count
			   again. */
			button_released_counter = 3;
		}
	} else {
		/* Check for button pressed. */
		if (button_value < BUTTON_PRESSED) {
			switch_off();
		}
	}
}


uint16_t get_10_bit_button_adc(void)
{
	uint8_t adcl;
	uint8_t adch;
	uint16_t ret;

	ADCSRA |= (1 << ADSC);
	do {
		/* nothing */
	} while (ADCSRA & (1 << ADSC));

	adcl = ADCL;
	adch = ADCH;
	ret = adch;
	ret <<= 2;
	ret |= (adcl >> 6);
	return ret;
}
