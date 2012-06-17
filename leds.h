#ifndef leds_h_INCLUDED
#define leds_h_INCLUDED

#include <avr/pgmspace.h>

#define LED1PORT PORTA
#define LED1PIN  PINA
#define LED1DDR  DDRA
#define LED1R    6		/* Pin 19 */
#define LED1G    5		/* Pin 18 */
#define LED1B    4		/* Pin 17 */

#define LED2PORT PORTA
#define LED2PIN  PINA
#define LED2DDR  DDRA
#define LED2R    3		/* Pin 16 */
#define LED2G    2		/* Pin 15 */
#define LED2B    1		/* Pin 14 */

#define LED3PORT PORTB
#define LED3PIN  PINB
#define LED3DDR  DDRB
#define LED3R    1		/* Pin 2 */
#define LED3G    2		/* Pin 3 */
#define LED3B    3		/* Pin 4 */

/* PB4 (DI, pin 5), PB5 (DO, pin 6), and PB6 (SCK, pin 7) are used for ICSP.
   We could use them for LEDS, but it's simplest to leave them uncommitted. */


struct FlashyLEDStatus {
	/** Pointer to the current part of the PWM sequence we are doing for
	    this LED.  This increments every time that pwmCounter overflows,
	    and when *pwmSequence is zero, we set pwmSequence to null and wait
	    for the main loop to decide to turn on this LED again. */
	const uint8_t *pwmSequence;
	/** Number of ticks this LED is required to be on. */
	uint8_t pwmOnTime;
	/** Up counter for the PWM cycle.  This is set to zero every time that
	    pwmOnTime is changed, and while it is less than pwmOnTime, the LED
	    is on.  When pwmCounter passes pwmOnTime, the LED is turned off. */
	uint8_t pwmCounter;
};


#define NLEDS 9
#define NPWMS 5


#define MAX_PWM_TIME 20


extern const uint8_t *pwmSequences[] PROGMEM;
extern volatile struct FlashyLEDStatus ledStatuses[];


void init_leds(void);
uint8_t getNLEDsOn(void);
uint8_t getTotalPWMBrightness(void);
void incLEDsOn(void);
void decLEDsOn(void);
void led_on(uint8_t led);
void led_off(uint8_t led);
uint8_t led_is_on(uint8_t led);

#endif
