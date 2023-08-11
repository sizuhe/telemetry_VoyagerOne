#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_HMC5883_U.h>
#include <Adafruit_BME680.h>

// ----- BME -----
#define SEALEVELPRESSURE_HPA 1013.25

Adafruit_BME680 bme;
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);   // Accelerometer
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified();    // Magnetometer

// Calibration variables
unsigned long REAL_TIME = 0;
float GRAVITY = 9.77;    // https://www.sensorsone.com/local-gravity-calculator/
float SCALE_Z = 1.0;
float offsetZ;



void sensors_begin() {
  if (!accel.begin()) {
    Serial.println("Accelerometer init failed");
  }
  if (!mag.begin()) {
    Serial.println("Magnetometer init failed");
  }
  if (!bme.begin()) {
    Serial.println("BME init failed");
  }

  // ----- SETTINGS -----
  accel.setRange(ADXL345_RANGE_16_G);
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}


void sensors_calibration() {
  // ----- ACCELEROMETER -----
  for (int i = 0; i < 100; i++) {
    sensors_event_t accelEvent;
    accel.getEvent(&accelEvent);
    offsetZ += accelEvent.acceleration.z - GRAVITY;
    delay(50);    // VALUE 50 IF MORE SENSORS
  }

  // ----- PROBAR SI TIENE SENTIDO -----
  // offsetZ /= 100.0;
  SCALE_Z = -1.0 / (offsetZ - GRAVITY);
}


String sensors_getData() {
  sensors_event_t accelEvent, magEvent;

  // Get data from sensors
  accel.getEvent(&accelEvent);
  mag.getEvent(&magEvent);

  // ----- ACCELEROMETER -----
  float accelZ = (accelEvent.acceleration.z - offsetZ) * SCALE_Z;

  // ----- MAGNETOMETER -----
  float heading = atan2(magEvent.magnetic.y, magEvent.magnetic.x);    // Degrees to magnetic north
  // MUST BE CHANGED ON LOCATION - Calibration of the magnetic field. http://www.magnetic-declination.com/
  float declinationAngle = -0.13;   // Radians
  heading += declinationAngle;
  if(heading < 0) {   // Correct for when signs are reversed.
    heading += 2*PI;
  }
  if(heading > 2*PI) {    // Check for wrap due to addition of declination.
    heading -= 2*PI;
  }
  float headingDegrees = heading * 180/M_PI;    // Convert radians to degrees for readability.
  float magneticTotal = sqrt(pow(magEvent.magnetic.x, 2) + pow(magEvent.magnetic.y, 2) + pow(magEvent.magnetic.z, 2));    // Magnetic total field

  // ----- BME -----
  bme.performReading();

  // ----- DATAPACKET INFO -----
  // humidity[%], temperature[ºc], pressure[hPa], altitude[m], acelZ[g], magTotal[uT], headDegrees[º], gasResistance
  String dataPacket = String(bme.humidity) + " " + String(bme.temperature) + " " + String(bme.pressure/100.0) + " " + String(bme.readAltitude(SEALEVELPRESSURE_HPA)) + " " 
                    + String(accelZ) + " " + String(magneticTotal) + " " + String(headingDegrees) + " " + String(bme.gas_resistance);

  return dataPacket;
}
