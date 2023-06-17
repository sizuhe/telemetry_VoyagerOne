/*
----- ERRORES ACTUALES -----
- SD no funciona en simultaneo con el LoRa.
- Al desconectarse de la energia y volverse a prender el sensor magnetico no se reinicia por lo que se debe volver a montar el codigo,
  este problema se observa ya que el sensor se inicializa desde void setup.
- GPS se tarda en triangular.
- Revisar exactitud de las variables y precision de la calibracion.
- Mirar que proceso se puede realizar en tierra
*/

#include <SPI.h>
#include <LoRa.h>
//#include <SD.h>
#include <TinyGPS++.h>
#include "sensors.h"
#include "encoders.h"

// ----- LORA SETTINGS -----
// SPI pin configuration
#define LORA_SCK 18
#define LORA_MISO 19
#define LORA_MOSI 23
#define LORA_NSS 5
#define LORA_RST 2
#define LORA_DI0 4
// ----------
#define LORA_FREQ 433E6   // LoRa frequency band

// ----- GPS PINS -----
// UART pin configuration
#define ESP_RX 16
#define ESP_TX 17

// ----- SD SETTINGS -----
// SPI pin configuration
// #define SD_SCK 14
// #define SD_MISO 26
// #define SD_MOSI 13
// #define SD_NSS 15

#define ENC_ANEM 34

// ----- Libraries instances -----
SPIClass LORA_SPI(VSPI);
// SPIClass SD_SPI(HSPI);
// File dataFile;
HardwareSerial gps_serial(1);
TinyGPSPlus gps;

void setup() {

  pinMode(ENC_ANEM , INPUT);
  attachInterrupt(digitalPinToInterrupt(ENC_ANEM),isr,RISING);

  // ----- INITIALIZATION -----
  Serial.begin(115200);
  gps_serial.begin(9600, SERIAL_8N1, ESP_RX, ESP_TX); // initialize Serial1 at 9600 baud, with 8 data bits, no parity, and 1 stop bit, using pins 16 (RX) and 17 (TX)
  LORA_SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_NSS);
  // SD_SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_NSS);
  
  // Modules pin configuration
  LoRa.setPins(LORA_NSS, LORA_RST, LORA_DI0);

  // ----- MODULES INITIALIZATION -----
  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("LoRa init failed");
  }
  // if (!SD.begin(SD_NSS)) {
  //   Serial.println("SD init failed");
  // }

  // Initialization and calibration of all modules - HAS TO BE AFTER LORA.BEGIN
  sensors_begin();
  sensors_calibration();
  // encoders_calibration();

  // ----- DATA FILE -----
  // dataFile = SD.open("/dataVolta.txt", FILE_WRITE);
  // if (!dataFile) {
  //   Serial.println("Error creating dataVolta.");
  // }
}

void loop() {

  while (gps_serial.available() > 0) {
    if (gps.encode(gps_serial.read())) {
      if (gps.location.isValid()) {
        float latitude = gps.location.lat();
        float longitude = gps.location.lng();
      }
    }
  }

  // DataBuffers from sensors
  String dataGPS = String(gps.location.lat(), 6) + "," + String(gps.location.lng(), 6);
  String dataSensors = sensors_getData();
  String dataEncoders = encoders_getData();

  // humidity[%], temperature[ºc], pressure[hPa], altitude[m], acelZ[g], magTotal[uT], headDegrees[º], gasResistance, lat, long, windrpm, windangle [º], anglequad. 
  String dataBuffer = dataSensors + "," + dataGPS + "," + dataEncoders;   // Main DataBuffer

  // Sending dataBuffer through LoRa
  LoRa.beginPacket();
  LoRa.print(dataBuffer);
  LoRa.endPacket();

  // Writing data to SD Card
  // dataFile.println(dataBuffer);
  // dataFile.flush();

  // ----- DEBUGGING -----
  Serial.println(dataBuffer);
}
