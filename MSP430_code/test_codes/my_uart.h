/*
 * my_uart.h
 *
 *  Created on: 30-Mar-2017
 *      Author: saurav
 */

#ifndef MY_UART_H_
#define MY_UART_H_

#include "commonheader.h"

void uart_init(void);
void uart_char(char data);
void uart_string(char *string);


#endif /* MY_UART_H_ */
