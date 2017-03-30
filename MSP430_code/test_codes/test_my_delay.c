/*
 * main.c
 */

#include "commonheader.h"
#include "my_delay.h"

void main(void)
{
    volatile unsigned int i ;

	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	P1DIR |= 0x01;
	P4DIR |= 0b10000000;


	while(1)
	{
		P1OUT ^= 0x01;
		P4OUT ^= 0b10000000;
//		delay_sec(1);
		delay_millisec(1000);
//		i++;
//		P1OUT = 0x00;
//		delay_sec(32000);

	}
}
