/*
 * co.h
 *
 *  Created on: Apr 2, 2017
 *      Author: parin
 */

#ifndef CO_H_
#define CO_H_
#include <stdint.h>
#define   Num_of_Results   8 //store 8 samples at a time
volatile uint8_t results[Num_of_Results]; //array to store sample values
void configure_ADC_12(void);
void start_conversion(void);


#endif /* CO_H_ */
