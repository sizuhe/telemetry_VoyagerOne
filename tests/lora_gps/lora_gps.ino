#include <SPI.h>
#include <LoRa.h>
#include <TinyGPS++.h>

// ----- LORA SETTINGS -----
// SPI pin configuration
// #d7
#define RA_SCK 18
// #define LORA_MISO 19
// #define LORA_MOSI 23
// #define LORA_NSS 5
// #define LORA_RST 2
// #define LORA_DI0 4
// // ----------
// #define LORA_FREQ 433E6   // LoRa frequency band

// ----- GPS PINS -----
// UART pin configuration
#define ESP_RX 16
#define ESP_TX 17

// ----- Libraries instances -----
// SPIClass LORA_SPI(VSPI);
HardwareSerial gps_serial(1);
TinyGPSPlus gps;


void setup() {

  // ----- INITIALIZATION -----
  Serial.begin(9600);
  gps_serial.begin(9600, SERIAL_8N1, ESP_RX, ESP_TX); // initialize Serial1 at 9600 baud, with 8 data bits, no parity, and 1 stop bit, using pins 16 (RX) and 17 (TX)
  // LORA_SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_NSS);
  
  // Modules pin configuration
  // LoRa.setPins(LORA_NSS, LORA_RST, LORA_DI0);

  // ----- MODULES INITIALIZATION -----
  // if (!LoRa.begin(LORA_FREQ)) {
  //   Serial.println("LoRa init failed");
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

  // humidity[%], temperature[ºc], pressure[hPa], altitude[m], acelZ[g], magTotal[uT], headDegrees[º], gasResistance, lat, long, windrpm, windangle [º], anglequad. 
 
  String dataBuffer = dataGPS;   // Main DataBuffer

  // Sending dataBuffer through LoRa
  // LoRa.beginPacket();
  // LoRa.print(dataBuffer);
  // LoRa.endPacket();

  // ----- DEBUGGING -----
  Serial.println(dataBuffer);
}
