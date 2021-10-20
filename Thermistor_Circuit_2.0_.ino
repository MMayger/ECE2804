#include <math.h>

#define analogpin A0
int R0 = 10000;
int T0 = 25; 
int B = 3380;
float TEMP;
float R;  //R is going to be found through use of the analog port on the arduino, so it will be: R = 5 - output of analog
int TK = 273.15;
int vout;
float voltage;
float i;
float microcurrent;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600); 

}

void loop() {
  // put your main code here, to run repeatedly: 
vout = analogRead(analogpin);   //measures the voltage at vout 
voltage = vout/204.6;           //converts analog output to actual voltage value 

i = (5-voltage)/R0;            //uses found voltage to determine current going into thermistor
microcurrent = i*1000000;      //converts found current to microA

R = voltage/i;                //Uses found voltage and current to get R  

//Uses Beta Equation provided in datasheet to solve for temperature
TEMP = 1/((log(R/R0)/B)+(1/(25+273.15)))-TK;

//Outputs Calculated V, I, R, and Temperature
Serial.print(" V = ");
Serial.print(voltage);
Serial.print("\t");
Serial.print(" I(uA) = ");
Serial.print(microcurrent);
Serial.print("\t");
Serial.print(" R = ");
Serial.print(R);
Serial.print("\t");
Serial.print(" T = ");
Serial.print(TEMP); 
Serial.print("\t");
delay(1000);
}
