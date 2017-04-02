#include <msp430.h> 
#include "co.h"
/*
 * main.c
 */


int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    configure_ADC_12();
    start_conversion();
    _bis_SR_register(LPM0_bits + GIE);       // Enter LPM4, Enable interrupts
         while(1);
	return 0;
}
