#include <Wire.h>

volatile double velocity = 0;


volatile double prevT = 0;
int increment = 1;
volatile double deltaT = 1000;

String predataBuffer = "1";


#define ENC_ANEM 34



void IRAM_ATTR isr() {

  volatile long currT = micros();

  deltaT = ((volatile float)(currT - prevT)) / 1.0e6;

  velocity = (increment / deltaT)*3;
           
  prevT = currT;

}


// ----- ENCODERS DATA -----
void encoders_calibration() {
  pinMode(ENC_ANEM, INPUT);
}

String encoders_getData() {
  
  String dataBuffer = String(velocity);

  if (dataBuffer == predataBuffer){

      dataBuffer = "0";
      
      return dataBuffer;

  } else{

    predataBuffer = dataBuffer;

    return dataBuffer;

  }



}
