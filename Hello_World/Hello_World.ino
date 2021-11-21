//Program Name: Send
//Author: Matthew Mayger
//ECE 2804
//This code reads the voltage accross a thermistor, converts that into a temperature reading and then uses the RFM69 module to transmit that data


//library declarations
#include <RFM69.h>
#include <RFM69registers.h>
#include <RFM69_ATC.h>
#include <RFM69_OTA.h>
#include <LowPower.h>
#include <SPI.h>


//constant definitions
#define DALLAS_PIN

#define FREQUENCY     RF69_433MHZ //tells Arduino the frequency band we’re operating in
#define CHANNEL       8 //defines frequency based on channelization of 433MHz band (see Matt's notebook for exact frequency numbers) THIS NUMBER MUST BE SAME ON BOTH RADIOS
#define NETWORKID     50 //all nodes must be on same network to talk to each other

#define GATEWAYID     1
#define NODEID        2 //All devices need a unique node ID
#define ACK_TIME      100 //time in ms waiting for acknowledgment from reciever
#define IS_RFM69HW_HCW
#define POWER         20 //radio power level integer -2 to 20 DBm

#define SERIAL_BAUD   115200 //set serial speed

//legacy pins for functionality of LED status bits. Not used in final circuit
#define REDLEDPIN 7
#define YELLOWLEDPIN 6
#define GREENLEDPIN 5

//thermistor defines
#define CONST_R0 10000 //value of resistor in the circuit
#define CONST_T0 25 // given temperature constant
#define CONST_B 3380 // calculated constant
#define THERM_PIN A0 // analog pin used to read voltage accross thermistor
#define CONST_TK 273.15 // constant used to convert from kelvin to celcius

//-----------------------------------------

RFM69 radio; //our RFM69 is now defined as the variable radio

void setup() {
  Serial.begin(SERIAL_BAUD); //sets data rate in bits per second

  startRadio();

  //set LED pins to output mode
  pinMode(REDLEDPIN, OUTPUT);
  pinMode(YELLOWLEDPIN, OUTPUT);
  pinMode(GREENLEDPIN, OUTPUT);

  //Blink all LEDs for debugging
  BlinkLED(GREENLEDPIN, 500);
  BlinkLED(YELLOWLEDPIN, 500);
  BlinkLED(REDLEDPIN, 500);

  //two functions used for debugging
  Serial.print ("Setup Complete\n");
  //radio.readAllRegs(); //Used to print out all registers for verification of RFM values

}

void startRadio(){ //initialization of radio functions
  radio.initialize(FREQUENCY, NODEID, NETWORKID); //initializes radio using defined parameters
  SetChannel(CHANNEL);
  radio.setHighPower(); //sets radio as high power which must be done for RFM69HCW(what we have)
  radio.setPowerDBm(POWER); //sets radio power to value defined in DBm
  
  //set bitrate to 1.2 kbps
  radio.writeReg(REG_BITRATEMSB, 0x68); 
  radio.writeReg(REG_BITRATELSB, 0x2B); 

  //used for debugging power level
  Serial.print(radio.getPowerLevel());
}

void SetChannel(int channelNo){ //sets frequency based on an input channel number. Valid inputs are integers 1 to 23
  long int frequency = 433125000 + 75000 * (channelNo-1); //calculates center frequency using channel integer long becuase number is outside range of int
  radio.setFrequency(frequency); //set frequency of radio

  //used for debugging frequency
  Serial.print("SET FREQUENCY: ");
  Serial.print(radio.getFrequency());
  Serial.print("\n");
}

void BlinkLED(byte PIN, int Delay){ //blinks LED will be used with debugging
  digitalWrite(PIN, HIGH);
  delay(Delay);
  digitalWrite(PIN, LOW);
}

float Read_Temp(){
  int vout = analogRead(THERM_PIN); //read voltage accross thermistor
  float voltage = vout/204.6; //convert arbitrary 1-1024 value to 0-5 volts

  float i = (5-voltage)/CONST_R0; //uses found voltage to determine current going into thermistor
  float R = voltage/i; //finds resistance of thermistor

  return 1/((log(R/CONST_R0)/CONST_B)+(1/(CONST_T0+CONST_TK)))-CONST_TK; //use Beta Equation provided in data sheet to solve for temperature
  
}

void loop() {
  char payload[6]; //string of characters comprised of two digits, a decimal, two degits, null char
  float Temp = Read_Temp();
  
  dtostrf(Temp, 6, 2, payload); //turn float temperature into a string of characters
  
  Serial.print(payload);
  if(radio.sendWithRetry(GATEWAYID, payload, strlen(payload))){ //send string in payload (an array of characters)
    BlinkLED(GREENLEDPIN, 1000); //blink green when send module recieves acknowledgement (radio.sendACK()) from receiver
    Serial.print("sent!\n"); //LEGACY for debugging
  }
  
  //BlinkLED(YELLOWLEDPIN, 5); //legacy used for debugging

}
