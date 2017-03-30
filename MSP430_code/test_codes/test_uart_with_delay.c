/*
 * test_uart_with_delay.c
 *
 *  Created on: 31-Mar-2017
 *      Author: saurav
 */

#include "commonheader.h"
#include "my_delay.h"
#include "my_uart.h"


void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	uart_init();

	while(1)
	{
		uart_char('S');
		uart_char('\n');
		uart_char('\r');
		delay_sec(1);
		uart_string("Saurav Shandilya");
		uart_char('\n');
		uart_char('\r');
		delay_sec(5);
	}
}
