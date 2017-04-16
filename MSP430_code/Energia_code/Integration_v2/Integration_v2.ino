#include <driverlib.h>
#include <stdint.h>
#include <DHT22_430.h>      // DHT22
#include <msp430.h>
#include "my_uart0.c"

const char node_id = 1;

//// DHT related variables
#define DHTPIN P2_0
DHT22 mySensor(DHTPIN);
boolean detect_dht;

volatile char incomingByte = 0;   // for incoming serial data

// CO related variables
#define   Num_of_Results   8 //store 8 samples at a time
volatile unsigned int i;
volatile float rs,ppm,voltage = 0;
volatile int flag=0;
volatile char flag_co = 0;
//volatile float ro = 313;
//volatile float rl=2500;
volatile uint8_t results[Num_of_Results]; //array to store sample values
volatile int index=0;
unsigned int humidity;
unsigned int datapacket[5]={0};

// voltage monitoring
volatile unsigned int batt_voltage=0;

unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 30000;//sampe 30s ;
unsigned long lowpulseoccupancy = 0;
float ratio = 0.0;
float concentration = 0.0;
float ug_m3 = 0.0;
unsigned int ratio1;
// data packet
//uint8_t datapacket[5]={65,66,677,306,'\r'};


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

void vout2_read(void)
{
  P4OUT=0x00;
  ratio=0.0;
  starttime = millis();//get the current time;
  lowpulseoccupancy=0.0;
  duration=0.0;
  digitalWrite(P3_6, HIGH);
  while(1)
  {
  duration = pulseIn(P4_2, LOW);
  lowpulseoccupancy = lowpulseoccupancy+duration;
  
  if ((millis()-starttime) > sampletime_ms)//if the sampel time == 30s
  {
  break;   
  }
  }
    //ratio = (lowpulseoccupancy-endtime+starttime)/(sampletime_ms*10.0);  // Integer percentage 0=>100
    ratio = ((lowpulseoccupancy/1000.0)/(sampletime_ms)) * 100;  // Integer percentage 0=>100
    //linear fitting function for Vout 2
  concentration = (0.1 * ratio) - 0.1; //from matlab linear
  ratio1=ratio*100;
    //concentration = 1.644 * ratio - 0.814;
    //concentration = 0.02*pow(ratio,2)-0.09*ratio - 0.03; // quadratic approximation
  ug_m3 = concentration*1000.0;
  
  //P4OUT=0x80;
 // delay(500);
   //Serial.print("1");
    //Serial.println("concentration = ");  
  
}

//void configure_ADC12_for_battery_voltage(void)
//{
//    P6SEL |= 0x0F;                            // Enable A/D channel A1 (digital pin P6.1 acting as A1)
//
//    REFCTL0 &= ~REFMSTR;                      // Reset REFMSTR to hand over control to
//                                               // ADC12_A ref control registers
//    ADC12MCTL0 = ADC12INCH_1 + ADC12SREF_1;                 // ref+=AVcc, channel = A1
//     ADC12CTL0 = ADC12ON+ADC12SHT02+ADC12REFON+ADC12REF2_5V + ADC12MSC;
//                                               // Turn on ADC12, Sampling time
//                                               // On Reference Generator and set to
//                                               // 2.5V
//     ADC12CTL1 = ADC12SHP + ADC12CONSEQ_2 + ADC12SSEL_1;                     // Use sampling timer
//     ADC12CTL2 = ADC12RES_0;
//     //ADC12MCTL0 = ADC12SREF_1 + ADC12RES_0;                 // Vr+=Vref+ and Vr-=AVss
//}
//
//void measure_battery_voltage(void)
//{
//  uint8_t data;
//  configure_ADC12_for_battery_voltage();
//  ADC12CTL0 |= ADC12ENC;                    // Enable conversions
//  ADC12CTL0 |= ADC12SC;                     // Start conversion
//      
//  while (!(ADC12IFG & BIT0));
//    batt_voltage = ADC12MEM0;
//    // data is converted into battery voltage at server
//    
////    batt_voltage = (0.009 * data);    
//}
/*
* Temperature and Humidity sensing --- STARTS
*/
void dht22_init(void)
{ 
  mySensor.begin();
}

int get_dht22_value(int *temperature, int *humidity)
{
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  detect_dht = mySensor.get();
  int32_t h = mySensor.humidityX10();
  int32_t t = mySensor.temperatureX10();
  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (!detect_dht) 
  {
    *temperature = 0;
    *humidity = 0;
  }
  
  else 
  {
    *temperature = t;
    *humidity = h;
  }
  //digitalWrite(P1_5,LOW);      // DHT MOSFET - Turn off
}

// Echo back RXed character, confirm TX buffer is ready first
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
    while (!(UCA0IFG & UCTXIFG));             // USCI_A0 TX buffer ready?
//    UCA1TXBUF = UCA1RXBUF;                  // TX -> RXed character
      incomingByte = UCA0RXBUF;
      //UCA0TXBUF = (UCA0RXBUF + 1);                  // TX -> RXed character
}

int get_co_data(void)
{  
  int sensorvalue,sum,avg, i;
  sum=0;
  //P4OUT=0x80;
  for (i=0;i<8;i++)
  {
  sensorvalue = analogRead(A0);
  sum = sum + sensorvalue;
  }
  //P4OUT=0x00;
  avg=sum/8;
  //digitalWrite(P2_5,LOW);      // CO MOSFET  - Turn off
 return avg;
}

void battery_voltage_measure(void)
{
  batt_voltage = analogRead(A1);
}
void setup()
{
  // put your setup code here, to run once:
  dht22_init();
  uart0_init();
  pinMode(P1_5,OUTPUT);
  pinMode(P2_5,OUTPUT);
  pinMode(P3_6,OUTPUT);
  pinMode(P4_2,OUTPUT);
  
//  digitalWrite(P1_5, LOW);
//    digitalWrite(P2_5, LOW);
//      digitalWrite(P3_6, LOW);
//        digitalWrite(P4_2, LOW);
  //low_power_setup();
 // pinMode(P1_0,OUTPUT);

//P4DIR=0x80;
//P4OUT=0x80;
  //uart0_string("Setup Done \n\r");
}

void loop()
{
  // put your main code here, to run repeatedly:
   unsigned int data;
   uint8_t data1,data2 = 0;
   unsigned int i = 0;
   unsigned int co_avg = 0;
   
   int temperature, humidity;
   
   digitalWrite(P1_5,HIGH);      // DHT MOSFET
   digitalWrite(P2_5,HIGH);      // CO MOSFET
   //digitalWrite(P3_6, HIGH);  // PM 
     
     //for(int i = 0; i<1000; i++);
   
   get_dht22_value(&temperature, &humidity);
   co_avg = get_co_data();
   vout2_read();
   //measure_battery_voltage();
   battery_voltage_measure();
   //datapacket[0] = '$';
   datapacket[0] = temperature;
   datapacket[1] = humidity;
   datapacket[2] = co_avg;
   datapacket[3] = ratio1;
   datapacket[4] = batt_voltage;
   //datapacket[3] = '\r';
   //datapacket[4] = '\n';
   
   if (incomingByte == 0x31)
   {  
      while(incomingByte != '#')
      {
        uart0_char('$');      // START of Packet - $
        uart0_char(node_id);  // Node ID - defined at top
        
        for (i=0; i<5; i++)
        {
          data = datapacket[i];
          //data = 55;
          data1=uint8_t(data/255);
          data2=uint8_t(data%256);
          uart0_char(data1);      //lower byte
          //delay(5);
          uart0_char(data2);       // higher byte
        }
        uart0_char('\r');      // END of Packet - \r followed by \n
        uart0_char('\n');      // END of Packet - \r followed by \n
        
//        digitalWrite(P1_5,LOW);      // DHT MOSFET
//        digitalWrite(P2_5,LOW);      // CO MOSFET
//        digitalWrite(P3_6, LOW);      // PM
      }
      //incomingByte = 0;
  }
  //datapacket[2] = {0};      
}
