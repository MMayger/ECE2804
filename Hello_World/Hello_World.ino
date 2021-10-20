//Program Name: Hello World
//Author: Matthew Mayger
//ECE 2804
//This code transmits Hello World as an example function to make sure the RF69 module is functioning correctly

//todo:
//troubleshoot why it never says sent but still sends
//fix string formatting
//add screen functionality


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
#define POWER         -2 //radio power level integer -2 to 20 DBm

#define SERIAL_BAUD   115200

#define REDLEDPIN 7
#define YELLOWLEDPIN 6
#define GREENLEDPIN 5

//-----------------------------------------

RFM69 radio; //our RFM69 is now defined as the variable radio

void setup() {
  Serial.begin(SERIAL_BAUD); //sets data rate in bits per second

  startRadio();
  
  pinMode(REDLEDPIN, OUTPUT);
  pinMode(YELLOWLEDPIN, OUTPUT);
  pinMode(GREENLEDPIN, OUTPUT);

  BlinkLED(GREENLEDPIN, 500);
  BlinkLED(YELLOWLEDPIN, 500);
  BlinkLED(REDLEDPIN, 500);

  Serial.print ("Setup Complete\n");
  radio.readAllRegs();

}

void startRadio(){ //initialization of radio functions
  radio.initialize(FREQUENCY, NODEID, NETWORKID); //initializes radio using defined parameters
  SetChannel(CHANNEL);
  radio.setHighPower(); //sets radio as high power which must be done for RFM69HCW(what we have)
  radio.setPowerDBm(POWER);

  radio.writeReg(REG_BITRATEMSB, 0x68); //set bitrate to 1.2 kbps
  radio.writeReg(REG_BITRATELSB, 0x2B); 
  Serial.print(radio.getPowerLevel());
}

void SetChannel(int channelNo){ //sets frequency based on an input channel number. Valid inputs are integers 1 to 23
  long int frequency = 433125000 + 75000 * (channelNo-1); //number is outside range of int
  radio.setFrequency(frequency);
  Serial.print("SET FREQUENCY: ");
  Serial.print(radio.getFrequency());
  Serial.print("\n");
}

void BlinkLED(byte PIN, int Delay){ //blinks LED will be used with debugging
  digitalWrite(PIN, HIGH);
  delay(Delay);
  digitalWrite(PIN, LOW);
}

char payload[] = "Hello World!!";

void loop() {
  if(radio.sendWithRetry(GATEWAYID, payload, strlen(payload))){ //send string in payload (an array of characters)
    BlinkLED(GREENLEDPIN, 1000); //blink green when send module recieves acknowledgement (radio.sendACK()) from receiver
    Serial.print("sent!\n"); //LEGACY for debugging
  }
  
  //BlinkLED(YELLOWLEDPIN, 5);

}
