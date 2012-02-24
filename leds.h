#ifndef leds_h_INCLUDED
#define leds_h_INCLUDED

#include <avr/pgmspace.h>

#define LED1PORT PORTA
#define LED1PIN  PINA
#define LED1DDR  DDRA
#define LED1R    6
#define LED1G    5
#define LED1B    4

#define LED2PORT PORTA
#define LED2PIN  PINA
#define LED2DDR  DDRA
#define LED2R    3
#define LED2G    2
#define LED2B    1

#define LED3PORT PORTB
#define LED3PIN  PINB
#define LED3DDR  DDRB
#define LED3R    0
#define LED3G    1
#define LED3B    2


struct FlashyLED {
	volatile uint8_t *port;	/**< Address of the PORTx register. */
	volatile uint8_t *ddr;	/**< Address of the DDRx register. */
	uint8_t bit;		/**< Bit mask in the registers. */
	uint8_t notbit;		/**< Inverted bit mask in the registers. */
};


struct FlashyLEDStatus {
	/** Pointer to the current part of the PWM sequence we are doing for
	    this LED.  This increments every time that pwmCounter overflows,
	    and when *pwmSequence is zero, we set pwmSequence to null and wait
	    for the main loop to decide to turn on this LED again. */
	uint8_t *pwmSequence;
	/** Number of ticks this LED is required to be on. */
	uint8_t pwmOnTime;
	/** Up counter for the PWM cycle.  This is set to zero every time that
	    pwmOnTime is changed, and while it is less than pwmOnTime, the LED
	    is on.  When pwmCounter passes pwmOnTime, the LED is turned off. */
	uint8_t pwmCounter;
};


#define NLEDS 9
#define NPWMSEQUENCES 5


extern const struct FlashyLED leds[] PROGMEM;
extern const uint8_t *pwmSequences[] PROGMEM;
extern volatile struct FlashyLEDStatus ledStatuses[];


void init_leds(void);


#endif
