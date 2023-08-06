#include <SPI.h>
#include <LoRa.h>

/* ----- Connections -----
| Arduino UNO | LoRa SX1278 |
|-------------|-------------|
| GND         | GND         |
| 3.3V        | 3.3V        |
| RST         | RST         |
| DI07        | DI00        |
| DI10        | NSS         |
| DI11        | MOSI        |
| DI12        | MISO        |
| DI13        | SCK         |
*/

// ----- LORA SETTINGS ------
#define LORA_FREQ 433E6     // LoRa frequency band



void setup() {
  Serial.begin(9600);

  // LoRa configuration
  LoRa.setGain(6);
  LoRa.setSignalBandwidth(62.5E3);
  LoRa.setSpreadingFactor(12);

  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("LoRa init failed");
    while (1);
  }

}


void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    int i = 0;
    char dataPacket[255];  // Buffer to hold data packet
    while (LoRa.available()) {
      dataPacket[i++] = (char)LoRa.read();    // Adds every character to a data array
      if (i >= sizeof(dataPacket)) {
        break;
      }
    }
    dataPacket[i] = '\0';  // End of dataPacket array
    Serial.println(dataPacket);
  }
}