/*
 * my_uart.c
 *
 *  Created on: 30-Mar-2017
 *      Author: saurav
 */

#include "commonheader.h"

void uart_init(void)
{
	P4SEL |= 0x30; 					//P4.4 and P4.5 USCI_A1 Tx and Rx
	UCA1CTL1 |= UCSWRST; 			// reset
	UCA1CTL1 |= UCSSEL_1;			// ACLK 32768

	UCA1BR0 = 3;					// baud rate 9600
	UCA1BR1 = 0x00;					// baud rate

	UCA1MCTL = 0x06;				//UCBRSx UCBRFx
	UCA1CTL0 = 0x00;

	UCA1CTL1 &= ~UCSWRST; 			// set

	UCA1IE |= UCRXIE;				// Enable USCI_A1 RX interrupt
}

// Echo back RXed character, confirm TX buffer is ready first
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
    while (!(UCA1IFG & UCTXIFG));             // USCI_A0 TX buffer ready?
//    UCA1TXBUF = UCA1RXBUF;                  // TX -> RXed character
    UCA1TXBUF = (UCA1RXBUF + 1);                  // TX -> RXed character
}

/*
 * uart_char()
 * Description: Print single character
 * Example uart_char('A'); prints letter A
 */
void uart_char(char data)
{
	UCA1TXBUF = data;
	while(UCA1STAT & UCBUSY);
}

/*
 * uart_string()
 * Description: Print string
 * Example: uart_string("Hello World");
 */
void uart_string(char *string)
{
	while(*string)
		uart_char(*string++);
	//UARTCharPut(UART0_BASE, '\n');
	//UARTCharPut(UART0_BASE, '\r');
}
