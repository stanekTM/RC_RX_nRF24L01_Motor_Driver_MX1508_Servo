
#include <SPI.h>          //https://github.com/arduino/ArduinoCore-avr/tree/master/libraries/SPI
#include <nRF24L01.h>     //https://github.com/nRF24/RF24
#include <RF24.h>         //https://github.com/nRF24/RF24
#include "PWMFrequency.h" //https://github.com/TheDIYGuy999/PWMFrequency
#include "ServoTimer2.h"  //https://github.com/nabontra/ServoTimer2 

const uint64_t my_radio_pipe = 0xE8E8F0F0E1LL; //The receiver address must be the same as the transmitter address

unsigned long lastReceiveTime = 0;

RF24 radio(8, 14); //Set CE and CSN pins (14 aka A0)

//Structure size max 32 bytes **********************************************************************************************
struct received_data
{
  byte ch1;
  byte ch2;
  byte ch3;
  byte ch4;
  byte ch5;
  byte ch6;
  byte ch7;
  byte ch8;
};

received_data data; //Create a variable with the above structure

//We will create variables with an initial integer *************************************************************************
int ch1_value = 0;
int ch2_value = 0;
int ch3_value = 0;
int ch4_value = 0;
int ch5_value = 0;
int ch6_value = 0;
int motA_value = 0;
int motB_value = 0;

//Reset values ​​(min = 0, mid = 127, max = 255) *****************************************************************************
void resetData()
{
  data.ch1 = 127;     
  data.ch2 = 127;
  data.ch3 = 127;
  data.ch4 = 127;
  data.ch5 = 0;
  data.ch6 = 0;
  data.ch7 = 127;
  data.ch8 = 127;
}

//Create servo object ******************************************************************************************************
ServoTimer2 servo1;
ServoTimer2 servo2;
ServoTimer2 servo3;
ServoTimer2 servo4;
ServoTimer2 servo5;
ServoTimer2 servo6;

void attachServoPins()
{
  servo1.attach(2);
  servo2.attach(3);
  servo3.attach(4);
  servo4.attach(7);
  servo5.attach(A4);
  servo6.attach(A5);
}

void outputServo()
{
  servo1.write(ch1_value);   
  servo2.write(ch2_value); 
  servo3.write(ch3_value);
  servo4.write(ch4_value);
  servo5.write(ch5_value);
  servo6.write(ch6_value);

  ch1_value = map(data.ch1,0,255,1000,2000); //linear
  ch2_value = map(data.ch2,0,255,1000,2000);
  ch3_value = map(data.ch3,0,255,1000,2000);
  ch4_value = map(data.ch4,0,255,1000,2000);
  ch5_value = map(data.ch5,0,1,1000,2000);
  ch6_value = map(data.ch6,0,1,1000,2000);
}

//Frequencies and motors control *******************************************************************************************
void outputPWM()
{  
/*
 * The base frequency for pins 3, 9, 10, 11 is 31250Hz.
 * The base frequency for pins 5, 6 is 62500Hz.
 * 
 * The divisors available on pins 5, 6, 9, 10 are: 1, 8, 64, 256, and 1024.
 * The divisors available on pins 3, 11       are: 1, 8, 32, 64, 128, 256, and 1024.
 * 
 * Pins 5, 6  are paired on timer0
 * Pins 9, 10 are paired on timer1
 * Pins 3, 11 are paired on timer2
 *  
 * PWM frequency (default)
 * D3  //pwm 488Hz, timer2, 8-bit
 * D11 //pwm 488Hz, timer2, 8-bit, SPI MOSI
   
 * D5  //pwm 976Hz, timer0, 8-bit
 * D6  //pwm 976Hz, timer0, 8-bit
 *
 * D9  //pwm 488Hz, timer1, 16-bit
 * D10 //pwm 488Hz, timer1, 16-bit     
*/
 
//PWM frequency pin D5 or pin D6:  1024 = 61Hz, 256 = 244Hz, 64 = 976Hz(default)
//MotorA (pin D5 or pin D6, prescaler 64)  
  setPWMPrescaler(5, 64);
  
//PWM frequency pin D9 or pin D10: 1024 = 30Hz, 256 = 122Hz, 64 = 488Hz(default), 8 = 3906Hz
//MotorB (pin D9 or pin D10, prescaler 8)  
  setPWMPrescaler(9, 8);

//MotorA -------------------------------------------------------------- 

  if (data.ch7 < 125)
  {
    motA_value = map(data.ch7, 125, 0, 0, 255);
    analogWrite(5, motA_value); 
    digitalWrite(6, LOW);
  }
  else if (data.ch7 > 129)
  {
    motA_value = map(data.ch7, 129, 255, 0, 255);
    analogWrite(6, motA_value); 
    digitalWrite(5, LOW);
  }
  else
  {
    digitalWrite(5, LOW); //"HIGH" brake, "LOW" no brake
    digitalWrite(6, LOW); //"HIGH" brake, "LOW" no brake
//    analogWrite(5, motA_value = 127); //adjustable brake (0-255)
//    analogWrite(6, motA_value = 127); //adjustable brake (0-255)
  }
  
//MotorB --------------------------------------------------------------

  if (data.ch8 < 125)
  {
    motB_value = map(data.ch8, 125, 0, 0, 255); 
    analogWrite(9, motB_value); 
    digitalWrite(10, LOW);
  }
  else if (data.ch8 > 129)
  {
    motB_value = map(data.ch8, 129, 255, 0, 255); 
    analogWrite(10, motB_value); 
    digitalWrite(9, LOW);
  }
  else
  {
//    digitalWrite(9, HIGH);  //"HIGH" brake, "LOW" no brake
//    digitalWrite(10, HIGH); //"HIGH" brake, "LOW" no brake
    analogWrite(9,  motB_value = 127);  //adjustable brake (0-255)
    analogWrite(10, motB_value = 127);  //adjustable brake (0-255)
  }
}

//Initial main settings ****************************************************************************************************
void setup()
{
  Serial.begin(9600);

  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(A1, OUTPUT);
  
  resetData(); //Reset each channel value

  //Define the radio communication
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
//  radio.setPALevel(RF24_PA_LOW);  
  radio.openReadingPipe(1, my_radio_pipe);
  
  radio.startListening(); //Set the module as receiver

  attachServoPins();
}

//Program loop *************************************************************************************************************
void loop()
{
  receive_the_data();

  //Check whether we keep receving data, or we have a connection between the two modules
  unsigned long now = millis();
  if (now - lastReceiveTime > 1000) //If the signal is lost, reset the data after 1 second
  {
    resetData(); //If connection is lost, reset the data
  }

  outputServo();
  outputPWM();
  led_RF(); 

  Serial.println(motB_value);   
}

//Reading data at a specific time ******************************************************************************************
void receive_the_data()
{
  if (radio.available()) //Check whether there is data to be received //while
  {
    radio.read(&data, sizeof(received_data)); //Read the whole data and store it into the 'data' structure
    lastReceiveTime = millis(); //At this moment we have received the data
  }
}  

//RF connection signaling **************************************************************************************************
void led_RF()
{
  if (radio.available())
  {
    digitalWrite(A1, LOW);
  }
  else
  {
    digitalWrite(A1, HIGH);
  }
}  
