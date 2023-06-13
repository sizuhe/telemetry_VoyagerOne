#include <SPI.h>
#include <SD.h>
#include <LoRa.h>

// #define LORA_SCK 18
// #define LORA_MISO 19
// #define LORA_MOSI 23
// #define LORA_NSS 5
// #define LORA_RST 2
// #define LORA_DI0 4
// #define SD_SCK 14
// #define SD_MISO 26
// #define SD_MOSI 13
// #define SD_NSS 15
#define LORA_SCK 14
#define LORA_MISO 26
#define LORA_MOSI 13
#define LORA_NSS 15
#define LORA_RST 2
#define LORA_DI0 4
#define SD_SCK 18
#define SD_MISO 19
#define SD_MOSI 23
#define SD_NSS 5
#define LORA_FREQ 433E6   // LoRa frequency band

SPIClass lora_spi(HSPI);
SPIClass sd_spi(VSPI);
File dataFile;

void setup() {
  Serial.begin(115200);
  lora_spi.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_NSS);
  sd_spi.begin(SD_SCK, SD_MISO, SD_MOSI, SD_NSS);
  LoRa.setPins(LORA_NSS, LORA_RST, LORA_DI0);

  // ----- MODULES INITIALIZATION -----
  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("LoRa init failed");
  }
  else {
    Serial.println("LoRa Error");
  }
  if (!SD.begin(SD_NSS)) {
    Serial.println("SD card initialization failed!");
  } 
  else {
    Serial.println("Divino todo");
  }

  dataFile = SD.open("/data.txt", FILE_WRITE);
  if (!dataFile) {
    Serial.println("Error creating file.");
    return;
  }
}



void loop() {
  // write data to file
  float sensorData = getSensorData(); // replace with your own sensor reading code
  LoRa.beginPacket();
  LoRa.print(sensorData);
  LoRa.endPacket();
  
  dataFile.println(sensorData);
  dataFile.flush(); // ensure that the data is written to the SD card
  // Serial.println("Data saved.");
  
  // wait a short time to avoid flooding the SD card with data
  delay(10);
}


float getSensorData() {
  // replace with your own sensor reading code
  return 0.0;
}
