//Program Name: Receive
//Author: Matthew Mayger
//ECE 2804
//This code recieves data and outputs a message to the attached display

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
#define POWER         20 //radio power level integer -2 to 20 DBm


#define SERIAL_BAUD   115200 //set serial speed

//legacy pins for functionality of LED status bits. Not used in final circuit
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

void startDisplay(){ //initialization of display functions
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //defines start register to begin reservation for display data
  display.display(); //display changes
  display.clearDisplay(); //clears display of default splash screen
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK); //sets text color to white on a black background
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

void displayWrite(char *message, int y, bool clearDisplay){ //writes message to display passed as string of characters
  if(clearDisplay){ //if the display clear is sent clear the display
    display.clearDisplay();
  }
  display.setCursor(10,y); //set the cursor at the beginning of the text string at a location of x = 10 and y = the input value to this function
  display.println(message); //print the given message to the display where teh cursor is centered
  display.display(); //display changes
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
  //Serial.print(rssi);
  char rssiSTR[10];
  strcpy(buff, "RSSI: "); //populates buffer with initial string
  strcat(buff, itoa(rssi, rssiSTR, 10)); //appends the conversion of the integer rssi to the string rssiSTR
  
  displayWrite(buff, 30, false); //writes the computed contents of the buffer
  BlinkLED(GREENLEDPIN, 5000); //blinks green LED to know receive was successful
  for (byte i = 0; i < radio.DATALEN; i++) //different method of displaying sent data to the serial monitor (LEGACY used for debugging)
     Serial.print((char)radio.DATA[i]);
}

void loop() {
  if(radio.receiveDone()){ //do the following if a transmission is received
    radio.sendACK(); //send an acknowledgment back to the transmitter
    radioDisplay(); //display the message the radio received
  }
  
  // used for debugging
  else displayWrite("Nothing yet", 10, true);
  BlinkLED(YELLOWLEDPIN, 500);

}
