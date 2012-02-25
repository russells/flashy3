#include "timer.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include "leds.h"
#include "toggle-pin.h"


void init_timer(void)
{
	// No OC0A or OC0B pin action
	// CTC mode
	// Clock = CLKio/64
	// Count to 16 for approx 1kHz interrupts

	cli();
	TCCR0A =(0b00 << COM0A0) |
		(0b00 << COM0B0) |
		(0b10 << WGM00);
	TCCR0B =(0 << WGM02) |
		(0b011 << CS00);
	OCR0A = 16;
	TIMSK0 = (1 << OCIE0A);
	sei();
}


SIGNAL(TIM0_COMPA_vect)
{
	uint8_t i;

	//TOGGLE_ON();

	for (i=0; i<NLEDS; i++) {
		volatile struct FlashyLEDStatus *ls = ledStatuses + i;
		if (! ls->pwmSequence) {
			continue;
		}
		ls->pwmCounter ++;
		if (ls->pwmCounter == ls->pwmOnTime) {
			led_off(i);
		}
		if (ls->pwmCounter >= MAX_PWM_TIME) {
			ls->pwmCounter = 0;
			ls->pwmSequence ++;
			ls->pwmOnTime = pgm_read_byte_near(ls->pwmSequence);
			if (! ls->pwmOnTime) {
				ls->pwmSequence = 0;
				ls->pwmCounter = 0;
				ls->pwmOnTime = 0;
			} else {
				led_on(i);
			}
		}
	}

	//TOGGLE_OFF();
}
