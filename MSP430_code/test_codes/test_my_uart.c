/*
 * test_my_uart.c
 *
 *  Created on: 30-Mar-2017
 *      Author: saurav
 */

#include "commonheader.h"
#include "my_uart.h"

void main(void)
{
    volatile unsigned int i ;
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	P1DIR |= 0x01;				// Port 1 pin 0 - LED1
	P4DIR |= 0b10000000;		// Port 4 pin 7 - LED2

	uart_init();

	char welcome_message[] = "Welcome to Serial Programming";


	while(welcome_message[i]!='\0')
	{
		UCA1TXBUF = welcome_message[i];
		while(UCA1STAT & UCBUSY);
	  		i++;
	}

	while(1)
	    {
			uart_char('S');
			uart_char('\n');
			uart_char('\r');
			uart_string("Saurav Shandilya");
			uart_char('\n');
			uart_char('\r');
	    }
}
