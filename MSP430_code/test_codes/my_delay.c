/*
 * my_delay.c
 *
 *  Created on: 30-Mar-2017
 *      Author: saurav
 *
 */

#include "commonheader.h"

void delay_sec(unsigned int number_of_seconds)
{
	TA0CCTL0 |= CM_0 + CCIE;	//Enable compare mode + Interrupts on Timer
	TA0CTL |= TASSEL_1;			//Use ACLK as source for timer - 32KHz

	unsigned int i = 0;
	unsigned int delayCycles = 32000; // for 1 sec delay Number of cycles in the timer

	TA0CCR0 = delayCycles;
	TA0CTL |= MC_1;			//Use UP mode timer

	for (i = 0; i<number_of_seconds; i++)
	{
		TA0R = 0;				// Timer/counter register
		while (TA0R < delayCycles)
			{
				_bis_SR_register(LPM0_bits + GIE); //Enter Low Power Mode 3 with interrupts
				_no_operation();
			}
	}
}

void delay_millisec(unsigned int number_of_milliseconds)
{
	TA0CCTL0 |= CM_0 + CCIE;	//Enable compare mode + Interrupts on Timer
	TA0CTL |= TASSEL_1;			//Use ACLK as source for timer - 32KHz

	unsigned int i = 0;
	unsigned int delayCycles = 32; // for 1 milli sec delay Number of cycles in the timer

	TA0CCR0 = delayCycles;
	TA0CTL |= MC_1;			//Use UP mode timer

	for (i = 0; i<number_of_milliseconds; i++)
	{
		TA0R = 0;				// Timer/counter register
		while (TA0R < delayCycles)
		{
			_bis_SR_register(LPM0_bits + GIE); //Enter Low Power Mode 3 with interrupts
			_no_operation();
		}
	}
}

/*
 * ISR for Timer0_A0
 */
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0(void)
{
	TA0CTL |= MC_0;			//Use UP mode timer
	__bic_SR_register_on_exit(LPM0_bits|GIE); //When we exit the interrupt routine we return to Low Power Mode

}
