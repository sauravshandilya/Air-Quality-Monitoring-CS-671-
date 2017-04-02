/*
 * co.c
 *
 *  Created on: Apr 2, 2017
 *      Author: parin
 */



#include <msp430.h> //exclude if common header is used
#include "co.h"
/*
 * configure_ADC_12
 * Description: Uses ADC_12 in 8 bit mode, 3.3 Volt reference and repeated conversion cycles.
 * makes adc use aclk=32 Khz
 */
void configure_ADC_12(void)
{
    P6SEL |= 0x01;                            // Enable A/D channel A0 (digital pin P6.0 acting as A0)
    ADC12CTL0 = ADC12SHT02 + ADC12ON + ADC12MSC + ADC12REF2_5V;         //16 clock cycles + core on + 2.5V reference
    ADC12CTL1 = ADC12SSEL_1 + ADC12CONSEQ_2 + ADC12SHP;   //select Aclk and repeated single channel
    ADC12CTL2 = ADC12RES_0; //8 bit mode
}


/*
 * start_conversion : self explanatory
 */
void start_conversion(void)
{
    ADC12IE = 0x01;                           // Enable ADC12IFG.0 (enable interrupt)
    ADC12CTL0 |= ADC12ENC;                    // Enable conversions
    ADC12CTL0 |= ADC12SC;                     // Start conversion
}


//ISR routine for ADC12
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12ISR (void)
{

    static unsigned char index = 0;
  switch(__even_in_range(ADC12IV,34))
  {
  case  0: break;                           // Vector  0:  No interrupt
  case  2: break;                           // Vector  2:  ADC overflow
  case  4: break;                           // Vector  4:  ADC timing overflow
  case  6:                                  // Vector  6:  ADC12IFG0
    results[index] = ADC12MEM0;             // Move results
    index++;                                // Increment results index, modulo; Set Breakpoint1 here

    if (index == 8)
    {
      //calculate_ppm();
        ADC12CTL0 = ADC12SC;

            ADC12IE = 0x00;
      __bic_SR_register_on_exit(CPUOFF);

    }
  case  8: break;                           // Vector  8:  ADC12IFG1
  case 10: break;                           // Vector 10:  ADC12IFG2
  case 12: break;                           // Vector 12:  ADC12IFG3
  case 14: break;                           // Vector 14:  ADC12IFG4
  case 16: break;                           // Vector 16:  ADC12IFG5
  case 18: break;                           // Vector 18:  ADC12IFG6
  case 20: break;                           // Vector 20:  ADC12IFG7
  case 22: break;                           // Vector 22:  ADC12IFG8
  case 24: break;                           // Vector 24:  ADC12IFG9
  case 26: break;                           // Vector 26:  ADC12IFG10
  case 28: break;                           // Vector 28:  ADC12IFG11
  case 30: break;                           // Vector 30:  ADC12IFG12
  case 32: break;                           // Vector 32:  ADC12IFG13
  case 34: break;                           // Vector 34:  ADC12IFG14
  default: break;
  }
}

