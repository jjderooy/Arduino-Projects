/*
 * Simple data logger. Make sure to run the SD formatting sketch first
 */
#include <SPI.h>
#include "SdFat.h"

#include <Wire.h>             // include Wire library, required for I2C devices
#include <Adafruit_Sensor.h>  // include Adafruit sensor library
#include <Adafruit_BMP280.h>  // include adafruit library for BMP280 sensor

// SD chip select pin.  Be sure to disable any other SPI devices such as Enet.
const uint8_t chipSelect = 4;

// Interval between data records in milliseconds.
// The interval must be greater than the maximum SD write latency plus the
// time to acquire and write data to the SD to avoid overrun errors.
// Run the bench example to check the quality of your SD card.
const uint32_t SAMPLE_INTERVAL_MS = 1000;

// Log file base name.  Must be six characters or less.
#define FILE_BASE_NAME "Data"
//------------------------------------------------------------------------------
// File system object.
SdFat sd;

// Log file.
SdFile file;

// Time in micros for next data record.
uint32_t logTime;
 
// define device I2C address: 0x76 or 0x77 (0x77 is library default address)
#define BMP280_I2C_ADDRESS  0x76
Adafruit_BMP280 bmp280;


// Write data header.
void writeHeader() {
  file.print(F("SECONDS,"));
  file.print(F("TEMP (C),"));
  file.print(F("PRESSURE (hPa),"));
  file.print(F("ALTITUDE (m)"));

  file.println();
}

// Log a data record.
void logData() {
  
  // get temperature, pressure and altitude from library
  float temperature = bmp280.readTemperature();  // get temperature
  float pressure    = bmp280.readPressure();     // get pressure
  float altitude_   = bmp280.readAltitude(1013.25); // get altitude (this should be adjusted to your local forecast)
  
  // Write data to file.  Start with log time in seconds.
  file.print(logTime / 1000000);
  file.write(',');
  file.print(temperature);
  file.write(',');
  file.print(pressure);
  file.write(',');
  file.print(altitude_);
  file.println();
}

// Error messages stored in flash.
#define error(msg) sd.errorHalt(F(msg))

void setup() {
  const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
  char fileName[13] = FILE_BASE_NAME "00.csv";

  Serial.begin(9600);

  // Wait for USB Serial
  while (!Serial) {
    SysCall::yield();
  }
  delay(1000);

  // Initialize at the highest speed supported by the board that is
  // not over 50 MHz. Try a lower speed if SPI errors occur.
  if (!sd.begin(chipSelect, SD_SCK_MHZ(50))) {
    sd.initErrorHalt();
  }

  // Find an unused file name.
  if (BASE_NAME_SIZE > 6) {
    error("FILE_BASE_NAME too long");
  }
  while (sd.exists(fileName)) {
    if (fileName[BASE_NAME_SIZE + 1] != '9') {
      fileName[BASE_NAME_SIZE + 1]++;
    } else if (fileName[BASE_NAME_SIZE] != '9') {
      fileName[BASE_NAME_SIZE + 1] = '0';
      fileName[BASE_NAME_SIZE]++;
    } else {
      error("Can't create file name");
    }
  }
  if (!file.open(fileName, O_WRONLY | O_CREAT | O_EXCL)) {
    error("file.open");
  }

  // Write data header.
  writeHeader();

  // Start on a multiple of the sample interval.
  logTime = micros()/(1000UL*SAMPLE_INTERVAL_MS) + 1;
  logTime *= 1000UL*SAMPLE_INTERVAL_MS;

  // Start the bmp280
  bmp280.begin(BMP280_I2C_ADDRESS);
}
//------------------------------------------------------------------------------
void loop() {
  // Time for next record.
  logTime += 1000UL*SAMPLE_INTERVAL_MS;

  // Wait for log time.
  int32_t diff;
  do {
    diff = micros() - logTime;
  } while (diff < 0);

  logData();

  // Force data to SD and update the directory entry to avoid data loss.
  if (!file.sync() || file.getWriteError()) {
    error("write error");
  }

}
