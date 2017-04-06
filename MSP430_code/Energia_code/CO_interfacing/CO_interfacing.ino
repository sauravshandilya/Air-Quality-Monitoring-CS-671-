#include <driverlib.h>
#include <stdint.h>
#define   Num_of_Results   8 //store 8 samples at a time

volatile unsigned int i;
volatile float rs,ppm,voltage = 0;
volatile int flag=0;

volatile float ro = 313;
volatile float rl=2500;

volatile uint8_t results[Num_of_Results]; //array to store sample values
int index=0;

void setup()
{
  Serial.begin(9600);
  Serial.println("dsad");
}

//ADC_10 bit in repeated single channel operation
void configure_ADC_12(void)
{
    P6SEL |= 0x01;                            // Enable A/D channel A0 (digital pin P6.0 acting as A0)
    ADC12CTL0 = ADC12SHT02 + ADC12ON + ADC12MSC + ADC12REF2_5V;         //16 clock cycles + core on + 2.5V reference
    ADC12CTL1 = ADC12SSEL_1 + ADC12CONSEQ_2 + ADC12SHP;   //select Aclk and repeated single channel
    ADC12CTL2 = ADC12RES_0;
}

void calculate_ppm(void)
{
    int i,sum=0, avg=0;

    for (i=8; i>0; i--)
    {
        sum = sum + results[i];
        //Serial.println(results[i]);
    }

    avg= sum/8;

    voltage = (0.012 * avg);
    //Serial.print("VOltage=");
    //Serial.println(voltage);

    rs = voltage * rl / ( 5 - voltage);

    ppm = (1/ (sqrt((rs/ro)/1.971))) ;
    
    Serial.print("PPM=");
    Serial.println(ppm);
    //ADC12CTL0 = ADC12SC;

    //ADC12IE = 0x00;
}

//int main(void) 
void loop ()
{
        delay(1000);  
  WDTCTL = WDTPW | WDTHOLD;  // Stop watchdog timer
    configure_ADC_12();
     P1DIR=0x01;
    P1OUT=0x00;
   
       ADC12IE = 0x01;                           // Enable ADC12IFG.0 (enable interrupt)
      ADC12CTL0 |= ADC12ENC;                    // Enable conversions
      ADC12CTL0 |= ADC12SC;                     // Start conversion

    __enable_interrupt();
//     Serial.println(results[index]);
      calculate_ppm();
      
}

#pragma vector=ADC12_VECTOR
__interrupt void ADC (void)
{

 results[index] = ADC12MEM0;// Move results

 P1OUT=0x01;
    index++;   
 
    if (index == 8)
    {
        index = 0;
        ADC12CTL0 = ADC12SC; //stop ADC

            //ADC12IE = 0x00;
  }
}

