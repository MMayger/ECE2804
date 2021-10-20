//Program Name: Hello World
//Author: Matthew Mayger
//ECE 2804
//This code recieves data and says what it got

//todo:
//find better way to reconstruct data
//fix string formating

//library declarations
#include <RFM69.h>
#include <RFM69registers.h>
#include <RFM69_ATC.h>
#include <RFM69_OTA.h>
#include <LowPower.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <Adafruit_SSD1306.h>
#include <splash.h>
#include <Wire.h>
#include <SPI.h>



#define FREQUENCY     RF69_433MHZ //tells Arduino the frequency band we’re operating in
#define CHANNEL       8 //defines frequency based on channelization of 433MHz band (see Matt's notebook for exact frequency numbers) THIS NUMBER MUST BE SAME ON BOTH RADIOS
#define NETWORKID     50 //all nodes must be on same network to talk to each other

#define GATEWAYID     1
#define NODEID        1 //All devices need a unique node ID
#define IS_RFM69HW_HCW
#define POWER         -1 //radio power level integer -2 to 20 DBm


#define SERIAL_BAUD   115200

#define REDLEDPIN 7
#define YELLOWLEDPIN 6
#define GREENLEDPIN 5

//-----------------------------------------

RFM69 radio; //our RFM69 is now defined as the variable radio
Adafruit_SSD1306 display(128, 64, &Wire, -1); //our display is now defined as an SSD1306

void setup() {
  Serial.begin(SERIAL_BAUD); //sets data rate in bits per second

  startRadio();
  startDisplay();
  
  pinMode(REDLEDPIN, OUTPUT);
  pinMode(YELLOWLEDPIN, OUTPUT);
  pinMode(GREENLEDPIN, OUTPUT);

  Serial.print ("Setup Complete\n");

}

void startDisplay(){ //initialization of display functions
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
}

void startRadio(){ //initialization of radio functions
  radio.initialize(FREQUENCY, NODEID, NETWORKID); //initializes radio using defined parameters
  SetChannel(CHANNEL); //channel is an integer from 1 to 34
  radio.setHighPower(); //sets radio as high power which must be done for RFM69HCW(what we have)
  radio.setPowerDBm(POWER); //power must be a value from -2 to 20
  Serial.print(radio.getPowerLevel()); //prints power level for debugging

  
  radio.writeReg(REG_BITRATEMSB, 0x68); //set bitrate to 1.2 kbps
  radio.writeReg(REG_BITRATELSB, 0x2B); 
}

void SetChannel(int channelNo){ //sets frequency based on an input channel number. Valid inputs are integers 1 to 23
  long int frequency = 433125000 + 75000 * (channelNo-1); //number is outside range of int
  radio.setFrequency(frequency);
  Serial.print("SET FREQUENCY: ");
  Serial.print(radio.getFrequency());
  Serial.print("\n");
}

void displayWrite(char *message, int y, bool clearDisplay){ //writes message to display passed as string of characters
  if(clearDisplay){
    display.clearDisplay();
  }
  display.setCursor(10,y);
  display.println(message);
  display.display();
}

void BlinkLED(byte PIN, int Delay){ //blinks LED will be used with debugging
  digitalWrite(PIN, HIGH);
  delay(Delay);
  digitalWrite(PIN, LOW);
}

void radioDisplay(){ //when radio.receiveDone is true this function happens
  displayWrite("You've got mail!", 10, true); //writes string to display clearing it first
  displayWrite((char*)radio.DATA, 20, false); //writes received data string to display not clearing previous

  char buff[20];
  int rssi = radio.RSSI; //sets RSSI to variable
  Serial.print(rssi);
  char rssiSTR[10];
  strcpy(buff, "RSSI: "); //populates buffer with initial string
  strcat(buff, itoa(rssi, rssiSTR, 10)); //appends the conversion of the integer rssi to the string rssiSTR
  
  displayWrite(buff, 30, false); //writes the computed contents of the buffer
  BlinkLED(GREENLEDPIN, 5000); //blinks green LED to know receive was successful
  for (byte i = 0; i < radio.DATALEN; i++) //different method of displaying sent data to the serial monitor (LEGACY used for debugging)
     Serial.print((char)radio.DATA[i]);
}

void loop() {
  if(radio.receiveDone()){
    radio.sendACK();
    radioDisplay();
  }
  else displayWrite("Nothing yet", 10, true);
  BlinkLED(YELLOWLEDPIN, 500);

}
