#include <SPI.h>
#include "SD.h"


// ----- HSPI -----
#define SD_SCK 14
#define SD_MISO 26
#define SD_MOSI 13
#define SD_CS 15

// ----- VSPI -----
// #define SD_SCK 18
// #define SD_MISO 19
// #define SD_MOSI 23
// #define SD_CS 5

SPIClass SD_SPI(HSPI);
// SPIClass SD_SPI(VSPI);
File dataFile;

// 4 MHz = 4000000, 10 MHz = 10000000
int SPI_SPEED = 10000000;


void setup() {
  Serial.begin(115200);
  SD_SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  if (!SD.begin(SD_CS, SD_SPI, SPI_SPEED)) {
    Serial.println("SD card initialization failed!");
  } 

  dataFile = SD.open("/data.txt", FILE_WRITE);
  if (!dataFile) {
    Serial.println("Error creating file.");
    return;
  }
}


void loop() {
  float sensorData = 0.0;
  
  dataFile.println(sensorData);
  dataFile.flush();
  // Serial.println("Data saved.");
  
  delay(100);
}