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

#define LED_BUILTIN 2

// Helper functions declarations
void checkIaqSensorStatus(void);
void errLeds(void);

// Create an object of the class Bsec
Bsec iaqSensor;

String output;

 double prevT_bme = 0;

 double currT_bme;

 double deltaT_bme;

 double preattitude = 0;

 double fallSpeed;

 double attitude;

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

  delay(1000);

  gps_serial.begin(9600, SERIAL_8N1, ESP_RX, ESP_TX); // initialize Serial1 at 9600 baud, with 8 data bits, no parity, and 1 stop bit, using pins 16 (RX) and 17 (TX)
  LORA_SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_NSS);
  SD_SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  iaqSensor.begin(0x77, Wire);

  iaqSensor.setTemperatureOffset(5.0f);   // ADJUST MANUALLY IN EACH OCATION 

  checkIaqSensorStatus();
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
  iaqSensor.run();

  currT_bme = micros();

  attitude = altitude(-100000);

  deltaT_bme = (currT_bme - prevT_bme) / 1.0e6;

  fallSpeed = (attitude - preattitude)/deltaT_bme ;

  preattitude = attitude;

  prevT_bme = currT_bme;

//  datetime, BME iaq[0-500], co2Equivalent [por rellenar], breathVolatilesEquivalent [por rellenar], pressure [Pa],], altitude [m], attitude [m] ,temperature [ºc], humidity[%], fallSpeed[m/s] ,acelZ[g], magTotal[uT], headDegrees[º] GPS [lat, long, altitude[m],speed[kph]] ENCONDER [windrpm] [windDirection]

 

  String dataBuffer = datetime + " " + String(iaqSensor.iaq) + " " + String(iaqSensor.co2Equivalent) + " " + String(iaqSensor.breathVocEquivalent) + " " + String(iaqSensor.pressure) + " " + String(altitude(1013.25))+ " " +  String(attitude) + " " + String(iaqSensor.temperature) + " " + String(iaqSensor.rawHumidity) + " " +  String(fallSpeed) + " " + dataSensors + " " + dataGPS + " " + dataEncoders + " " + dataWindDir;   // Main DataBuffer



  // Sending dataBuffer through LoRa
  LoRa.beginPacket();
  LoRa.print(dataBuffer);
  LoRa.endPacket();

  // Writing data to SD Card
  save_data_sd(datetime,dataBuffer);
  
  // ----- DEBUGGING -----
  Serial.println(dataBuffer);

  delay(1000);
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


float altitude(float SL){

    static float start =  iaqSensor.pressure / 100.0f ;

    if (SL == -100000){

       float atmospheric = iaqSensor.pressure/ 100.0f;

       return 44330.0 * (1.0 - pow(atmospheric/start, 0.1903));   //PRESSURE SEA LEV

    }else {

        float atmospheric = iaqSensor.pressure/ 100.0f;

        return 44330.0 * (1.0 - pow(atmospheric/SL, 0.1903));   //PRESSURE SEA LEV

      } 


}

