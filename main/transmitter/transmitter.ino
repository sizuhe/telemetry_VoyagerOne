/*
----- ERRORES ACTUALES -----
- GPS se tarda en triangular.
- Revisar exactitud de las variables y precision de la calibracion.
- Mirar que proceso se puede realizar en tierra.
*/

#include <SPI.h>
#include <LoRa.h>
#include <TinyGPS++.h>
#include <SD.h>
#include "sensors.h"
#include "encoders.h"
#include "gps.h"
#include "sd.h"
#include "bsec.h"


// ----- BME680 SETTINGS -----

// Helper functions declarations
void checkIaqSensorStatus(void);
void errLeds(void);

// Create an object of the class Bsec
Bsec iaqSensor;

String output;



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
#define SD_SCK 14
#define SD_MISO 26
#define SD_MOSI 13
#define SD_CS 15

#define ENC_ANEM 34

// ----- Libraries instances -----
SPIClass LORA_SPI(VSPI);
SPIClass SD_SPI(HSPI);

/*
4 MHz = 4000000, 10 MHz = 10000000

The SD Module has a default SPI speed of 4 MHz and LoRa has a default speed of 10 MHz.
To work simultaneously in the ESP32 they need to be configured to have the same SPI Speed.
*/

int SPI_SPEED = 10000000;



void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ENC_ANEM , INPUT);
  attachInterrupt(digitalPinToInterrupt(ENC_ANEM),isr,RISING);

  // ----- INITIALIZATION -----
  Serial.begin(9600);
  gps_serial.begin(9600, SERIAL_8N1, ESP_RX, ESP_TX); // initialize Serial1 at 9600 baud, with 8 data bits, no parity, and 1 stop bit, using pins 16 (RX) and 17 (TX)
  LORA_SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_NSS);
  SD_SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  iaqSensor.begin(0x77, Wire);
  checkIaqSensorStatus();
  output = "\nBSEC library version " + String(iaqSensor.version.major) + "." + String(iaqSensor.version.minor) + "." + String(iaqSensor.version.major_bugfix) + "." + String(iaqSensor.version.minor_bugfix);
  Serial.println(output);
  bsec_virtual_sensor_t sensorList[13] = {
    BSEC_OUTPUT_IAQ,
    BSEC_OUTPUT_STATIC_IAQ,
    BSEC_OUTPUT_CO2_EQUIVALENT,
    BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
    BSEC_OUTPUT_RAW_TEMPERATURE,
    BSEC_OUTPUT_RAW_PRESSURE,
    BSEC_OUTPUT_RAW_HUMIDITY,
    BSEC_OUTPUT_RAW_GAS,
    BSEC_OUTPUT_STABILIZATION_STATUS,
    BSEC_OUTPUT_RUN_IN_STATUS,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
    BSEC_OUTPUT_GAS_PERCENTAGE
  };

  iaqSensor.updateSubscription(sensorList, 13, BSEC_SAMPLE_RATE_LP);
  checkIaqSensorStatus();


  // Modules pin configuration
  LoRa.setPins(LORA_NSS, LORA_RST, LORA_DI0);
  LoRa.setTxPower(20);
  LoRa.setSignalBandwidth(9);
  LoRa.setSpreadingFactor(12);


  // ----- MODULES INITIALIZATION -----
  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("LoRa init failed");
  }
  if (!SD.begin(SD_CS, SD_SPI, SPI_SPEED)) {
    Serial.println("SD init failed");
  }

  // Initialization and calibration of all modules - HAS TO BE AFTER LORA.BEGIN
  sensors_begin();
  sensors_calibration();
  encoders_calibration();

  // ----- DATA FILE -----
  gps_updateData();
  datetime = gps_getDatetime();
  create_file_sd(datetime);
}


void loop() {
  gps_updateData();
  datetime = gps_getDatetime();
  dataGPS = gps_getLocation();
  dataWindDir = gps_getWindDirection();
  String dataSensors = sensors_getData();
  String dataEncoders = encoders_getData();

  unsigned long time_trigger = millis();
  if (iaqSensor.run()) { // If new data is available
    digitalWrite(LED_BUILTIN, LOW);
    output = String(time_trigger);
    output += ", " + String(iaqSensor.iaq);
    output += ", " + String(iaqSensor.iaqAccuracy);
    output += ", " + String(iaqSensor.staticIaq);
    output += ", " + String(iaqSensor.co2Equivalent);
    output += ", " + String(iaqSensor.breathVocEquivalent);
    output += ", " + String(iaqSensor.rawTemperature);
    output += ", " + String(iaqSensor.pressure);
    output += ", " + String(iaqSensor.rawHumidity);
    output += ", " + String(iaqSensor.gasResistance);
    output += ", " + String(iaqSensor.stabStatus);
    output += ", " + String(iaqSensor.runInStatus);
    output += ", " + String(iaqSensor.temperature);
    output += ", " + String(iaqSensor.humidity);
    output += ", " + String(iaqSensor.gasPercentage);
    Serial.println(output);
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    checkIaqSensorStatus();
  }


  // BME680 [humidity[%], temperature[ºc], pressure[hPa], altitude[m], acelZ[g], magTotal[uT], headDegrees[º], gasResistance] | GPS [lat, long, altitude[m], speed[kph]] | ENCONDER [windrpm], [windDirection]. 
  String dataBuffer = datetime + " " + dataSensors + " " + dataGPS + " " + dataEncoders + " " + dataWindDir;   // Main DataBuffer

  // Sending dataBuffer through LoRa
  LoRa.beginPacket();
  LoRa.print(dataBuffer);
  LoRa.endPacket();

  // Writing data to SD Card
  save_data_sd(datetime,dataBuffer);
  
  // ----- DEBUGGING -----
  Serial.println(dataBuffer);
}


void checkIaqSensorStatus(void)
{
  if (iaqSensor.bsecStatus != BSEC_OK) {
    if (iaqSensor.bsecStatus < BSEC_OK) {
      output = "BSEC error code : " + String(iaqSensor.bsecStatus);
      Serial.println(output);
      for (;;)
        errLeds(); /* Halt in case of failure */
    } else {
      output = "BSEC warning code : " + String(iaqSensor.bsecStatus);
      Serial.println(output);
    }
  }

  if (iaqSensor.bme68xStatus != BME68X_OK) {
    if (iaqSensor.bme68xStatus < BME68X_OK) {
      output = "BME68X error code : " + String(iaqSensor.bme68xStatus);
      Serial.println(output);
      for (;;)
        errLeds(); /* Halt in case of failure */
    } else {
      output = "BME68X warning code : " + String(iaqSensor.bme68xStatus);
      Serial.println(output);
    }
  }
}

void errLeds(void)
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}