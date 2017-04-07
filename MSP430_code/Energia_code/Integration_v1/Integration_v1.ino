#include <driverlib.h>
#include <stdint.h>
#include <DHT22_430.h>      // DHT22
#include <SoftwareSerial.h>  // xbee interface

// Serial setup for zigbee
// serial setup
SoftwareSerial uart0(P3_4, P3_3); // RX, TX

// DHT related variables
#define DHTPIN P2_0
DHT22 mySensor(DHTPIN);
boolean detect_dht;

// CO related variables
#define   Num_of_Results   8 //store 8 samples at a time
volatile unsigned int i;
volatile float rs,ppm,voltage = 0;
volatile int flag=0;
volatile float ro = 313;
volatile float rl=2500;
volatile uint8_t results[Num_of_Results]; //array to store sample values
int index=0;

void low_power_setup(void)
{
    GPIO_setAsOutputPin(GPIO_PORT_P1,GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6);    // 0-6 pins
    GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);    // 0-7 pins
    GPIO_setAsOutputPin(GPIO_PORT_P3,GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);    // 0-7 pins
    GPIO_setAsOutputPin(GPIO_PORT_P4,GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);    // 0-7 pins
    GPIO_setAsOutputPin(GPIO_PORT_P6,GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6);    // 0-6 pins
    GPIO_setAsOutputPin(GPIO_PORT_P7,GPIO_PIN0|GPIO_PIN4);    // 0,4 pins
    GPIO_setAsOutputPin(GPIO_PORT_P8,GPIO_PIN1|GPIO_PIN2);    // 1,2 pins
    GPIO_setAsOutputPin(GPIO_PORT_PJ,GPIO_PIN0);    // 0-7 pins;
    
    GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6);    // 0-6 pins
    GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);    // 0-7 pins
    GPIO_setOutputHighOnPin(GPIO_PORT_P3,GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);    // 0-7 pins
    GPIO_setOutputHighOnPin(GPIO_PORT_P4,GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);    // 0-7 pins
    GPIO_setOutputHighOnPin(GPIO_PORT_P6,GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6);    // 0-7 pins
    GPIO_setOutputHighOnPin(GPIO_PORT_P7,GPIO_PIN0|GPIO_PIN4);    // 0,4 pins
    GPIO_setOutputHighOnPin(GPIO_PORT_P8,GPIO_PIN1|GPIO_PIN2);    // 1,2 pins
    GPIO_setOutputHighOnPin(GPIO_PORT_PJ,GPIO_PIN0);    // 1,2 pins
}

void dht22_setup(void)
{ 
  boolean detect_dht;
  mySensor.begin();
}

void get_dht22_value_debug() 
{
  Serial.println("Reading Temperature and Humidity - Debug Mode");
  delay(2000);      // use sleep instead of delay
 // sleep(2000);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  detect_dht = mySensor.get();
  int32_t h = mySensor.humidityX10();
  int32_t t = mySensor.temperatureX10();
  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (!detect_dht) 
  {
    Serial.println("Failed to read from DHT.. Check Pin connections");
  }
  
  else 
  {
    Serial.print("Humidity=");
    Serial.print(h/10);
    Serial.print(".");
    Serial.print(h%10);
    Serial.println("%");
    
    Serial.print("Temprature=");
    Serial.print(t/10);
    Serial.print(".");
    Serial.print(t%10);
    Serial.println("C");
  }
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
    //Serial.print("Rs");
    //Serial.println(rs);

    ppm = (1/ (sqrt((rs/ro)/1.971))) ;
    
    Serial.print("PPM=");
    Serial.println(ppm);
    //ADC12CTL0 = ADC12SC;

    //ADC12IE = 0x00;
}

void setup()
{
  dht22_setup();
  Serial.begin(9600);
}

void loop ()
{ 
  //  get_dht22_value();
  get_dht22_value_debug();
  //delay(2000)
  

    configure_ADC_12();
     P1DIR=0x01;
    P1OUT=0x00;
   
       ADC12IE = 0x01;                           // Enable ADC12IFG.0 (enable interrupt)
      ADC12CTL0 |= ADC12ENC;                    // Enable conversions
      ADC12CTL0 |= ADC12SC;                     // Start conversion

    __enable_interrupt();
//     Serial.println(results[index]);
      calculate_ppm();
      for(int z = 0; z<10000;z++);    
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
  }
}

