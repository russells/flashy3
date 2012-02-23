

#include <avr/io.h>
#include "flashy3.h"


static void setup_ports(void);


int main(void)
{
	setup_ports();
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
