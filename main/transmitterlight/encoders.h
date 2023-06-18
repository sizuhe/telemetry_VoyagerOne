#include <Wire.h>

volatile int cont = 0;
volatile int rpm = 0;
volatile int viento= 0;
String windspeed = " ";
unsigned long previousTime = 0;
unsigned long currentTime = 0;
bool state = LOW;
bool prevState = LOW;

int magnetStatus = 0; //value of the status register (MD, ML, MH)
int lowbyte; //raw angle 7:0
word highbyte; //raw angle 7:0 and 11:8
int rawAngle; //final raw angle 
float degAngle; //raw angle in degrees (360/4096 * [value between 0-4095])
int quadrantNumber, previousquadrantNumber; //quadrant IDs
float numberofTurns = 0; //number of turns
float correctedAngle = 0; //tared angle - based on the startup value
float startAngle = 0; //starting angle
float totalAngle = 0; //total absolute angular displacement
float previoustotalAngle = 0; //for the display printing

#define ENC_ANEM 34


// ------ ANENOMETER -----
String speedAnemometer() {
    rpm = (60*cont)/20;
    viento =  (rpm*6.2832/60)*(0.025);
    cont = 0;  
    String dataSpeed = String(rpm) + "," + String(viento);

    return dataSpeed;
}


void countAnemometerPulses() {
  state = digitalRead(ENC_ANEM);
  if (state == HIGH && prevState == LOW) {
    cont = cont+1;
  }
  prevState = state;
}





// ----- ANGLE -----
void readRawAngle() { 
  //7:0 - bits
  Wire.beginTransmission(0x36); //connect to the sensor
  Wire.write(0x0D); //figure 21 - register map: Raw angle (7:0)
  Wire.endTransmission(); //end transmission
  Wire.requestFrom(0x36, 1); //request from the sensor
  
  while(Wire.available() == 0); //wait until it becomes available 
  lowbyte = Wire.read(); //Reading the data after the request
   
  //11:8 - 4 bits
  Wire.beginTransmission(0x36);
  Wire.write(0x0C); //figure 21 - register map: Raw angle (11:8)
  Wire.endTransmission();
  Wire.requestFrom(0x36, 1);
  
  while(Wire.available() == 0);  
  highbyte = Wire.read();
  
  //4 bits have to be shifted to its proper place as we want to build a 12-bit number
  highbyte = highbyte << 8; //shifting to left
  //What is happening here is the following: The variable is being shifted by 8 bits to the left:
  //Initial value: 00000000|00001111 (word = 16 bits or 2 bytes)
  //Left shifting by eight bits: 00001111|00000000 so, the high byte is filled in
  
  //Finally, we combine (bitwise OR) the two numbers:
  //High: 00001111|00000000
  //Low:  00000000|00001111
  //      -----------------
  //H|L:  00001111|00001111
  rawAngle = highbyte | lowbyte; //int is 16 bits (as well as the word)

  //We need to calculate the angle:
  //12 bit -> 4096 different levels: 360° is divided into 4096 equal parts:
  //360/4096 = 0.087890625
  //Multiply the output of the encoder with 0.087890625
  degAngle = rawAngle * 0.087890625;
}


float correctAngle() {
  //recalculate angle
  correctedAngle = degAngle - startAngle; //this tares the position

  if (correctedAngle < 0) {  //if the calculated angle is negative, we need to "normalize" it
  correctedAngle = correctedAngle + 360; //correction for negative numbers (i.e. -15 becomes +345)
  }

  return correctedAngle;
}


int checkQuadrant() {
  /*
  //Quadrants:
  4  |  1
  ---|---
  3  |  2
  */

  //Quadrant 1
  if(correctedAngle >= 0 && correctedAngle <=90) {
    quadrantNumber = 1;
  }

  //Quadrant 2
  if(correctedAngle > 90 && correctedAngle <=180) {
    quadrantNumber = 2;
  }

  //Quadrant 3
  if(correctedAngle > 180 && correctedAngle <=270) {
    quadrantNumber = 3;
  }

  //Quadrant 4
  if(correctedAngle > 270 && correctedAngle <360) {
    quadrantNumber = 4;
  }
  
  return quadrantNumber;
}  


void checkMagnetPresence() {  
  //This function runs in the setup() and it locks the MCU until the magnet is not positioned properly

  while((magnetStatus & 32) != 32) {  //while the magnet is not adjusted to the proper distance - 32: MD = 1
    magnetStatus = 0; //reset reading

    Wire.beginTransmission(0x36); //connect to the sensor
    Wire.write(0x0B); //figure 21 - register map: Status: MD ML MH
    Wire.endTransmission(); //end transmission
    Wire.requestFrom(0x36, 1); //request from the sensor

    while(Wire.available() == 0); //wait until it becomes available 
    magnetStatus = Wire.read(); //Reading the data after the request

    Serial.print("Magnet status: ");
    Serial.println(magnetStatus, BIN); //print it in binary so you can compare it to the table (fig 21)      
  }  
  //Status register output: 0 0 MD ML MH 0 0 0  
  //MH: Too strong magnet - 100111 - DEC: 39 
  //ML: Too weak magnet - 10111 - DEC: 23     
  //MD: OK magnet - 110111 - DEC: 55

  Serial.println("Magnet found!");
  delay(1000);  
}





// ----- ENCODERS DATA -----
void encoders_calibration() {
  pinMode(ENC_ANEM, INPUT);

  checkMagnetPresence(); //check the magnet (blocks until magnet is found)
  readRawAngle(); //make a reading so the degAngle gets updated
  startAngle = degAngle; //update startAngle with degAngle - for taring  
}


String encoders_getData() {
  countAnemometerPulses();
  readRawAngle();

  currentTime = millis();
  if (currentTime - previousTime >= 1000) {
    previousTime = currentTime; 
    windspeed = speedAnemometer();
  }

  String dataBuffer = String(windspeed) + "," + String (correctAngle()) + "," + String(checkQuadrant());
  
  return dataBuffer;
}