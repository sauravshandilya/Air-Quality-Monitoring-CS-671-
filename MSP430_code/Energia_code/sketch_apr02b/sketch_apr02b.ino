#include <SoftwareSerial.h>  // xbee interface
#include <DHT22_430.h>      // DHT22
#include <driverlib.h>

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

// serial setup
SoftwareSerial uart0(P3_4, P3_3); // RX, TX

// dht22 setup
#define DHTPIN P2_0
DHT22 mySensor(DHTPIN);
boolean flag;

void uart0_setup(void)
{
  
  // set the data rate for the SoftwareSerial port
  uart0.begin(9600);
}

void dht22_setup(void)
{
  
  boolean flag;
  mySensor.begin();
}

void get_dht22_value() 
{
  delay(2000);      // use sleep instead of delay
 // sleep(2000);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  flag = mySensor.get();
  int32_t h = mySensor.humidityX10();
  int32_t t = mySensor.temperatureX10();
  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (!flag) 
  {
    uart0.println("Failed to read from DHT");
  }
  
  else 
  {
    uart0.print(h/10);
    uart0.print(".");
    uart0.println(h%10);
    
    uart0.print(t/10);
    uart0.print(".");
    uart0.println(t%10);
  }
}

void get_dht22_value_debug() 
{
  Serial.println("Reading Temperature and Humidity - Debug Mode");
  delay(2000);      // use sleep instead of delay
 // sleep(2000);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  flag = mySensor.get();
  int32_t h = mySensor.humidityX10();
  int32_t t = mySensor.temperatureX10();
  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (!flag) 
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

void setup()  
{
  //low_power_setup();
  uart0_setup();
  dht22_setup();
  Serial.begin(9600);
}

void loop() // run over and over
{
  sleep(2000);
  //uart0.println("Reading..."); 
   
//  get_dht22_value();
  get_dht22_value_debug();
  
//   mySerial.write(mySerial.read());
}


