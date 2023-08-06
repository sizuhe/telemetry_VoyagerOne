#include <SPI.h>
#include <LoRa.h>
#include "SD.h"

/*
Se deben probar y analizar los efectos en rendimiento que pueda llegar a tener
la disminucion de la frecuencia del SPI.

Despues de retirar la SD, la ESP32 debe apagarse y encenderse con la SD para que esta
guarde datos, de no hacerse asi, la SD no guardara datos correctamente.
*/

// ----- LORA SETTINGS -----
// SPI pin configuration
#define LORA_SCK 18
#define LORA_MISO 19
#define LORA_MOSI 23
#define LORA_CS 5
#define LORA_RST 2
#define LORA_DI0 4
// ----------
#define LORA_FREQ 433E6   // LoRa frequency band

// ----- SD PINS FOR HSPI -----
#define SD_SCK 14
#define SD_MISO 26
#define SD_MOSI 13
#define SD_CS 15

SPIClass SD_SPI(HSPI);
SPIClass LORA_SPI(VSPI);
File dataFile;

/*
4 MHz = 4000000, 10 MHz = 10000000

The SD Module has a default SPI speed of 4 MHz and LoRa has a default speed of 10 MHz.
To work simultaneously in the ESP32 they need to be configured to have the same SPI Speed.
*/

int SPI_SPEED = 10000000;



void setup() {
  Serial.begin(9600);
  SD_SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  LORA_SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);

  // Lora configuration
  LoRa.setPins(LORA_CS, LORA_RST, LORA_DI0);

  // ----- MODULES INITIALIZATION -----
  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("LoRa init failed");
  }

  if (!SD.begin(SD_CS, SD_SPI, SPI_SPEED)) {
    Serial.println("SD card initialization failed!");
  }

  dataFile = SD.open("/data.txt", FILE_WRITE);
  if (!dataFile) {
    Serial.println("Error creating file");
    return;
  }
}


void loop() {
  float sensorData = 0.0;

  dataFile.println(sensorData);
  dataFile.flush();

  LoRa.beginPacket();
  LoRa.print(sensorData);
  LoRa.endPacket();
}