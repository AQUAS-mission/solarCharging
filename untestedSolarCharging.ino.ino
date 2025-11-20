#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <esp_sleep.h>
#include <esp_wifi.h>
#include <esp_bt.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <WiFi.h>
#include <driver/rtc_io.h>
#include <Adafruit_INA260.h>
#include <TimeLib.h>

const int sdCardCSPin = 5;
String filename = "/volt.csv";
Adafruit_INA260 ina260 = Adafruit_INA260();
tm local;

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Sensor Buoy Starting...");
  if(!ina260.begin())
  {
    Serial.println("Couldn't find INA260 chip");
    while(1);
  }
  Serial.println("Found INA260 chip");

  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_UNDEFINED) { // if reset or power loss
    struct timeval val;
    loadStruct(&local); // e.g. load time from eeprom
    const time_t sec = mktime(&local); // make time_t
    localtime(&sec); //set time
  }   
  else {
  getLocalTime(&local);
  }
  
  initSD();

}

void initSD() {
  // Power on SD card first
  
  // Initialize SD card
  Serial.print("Initializing SD card...");
  if (!SD.begin(sdCardCSPin)) {
    Serial.println("SD card failed, or not present");
    Serial.println("Check wiring and SD card");
    while (1)
      ;
  }
  Serial.println("SD card initialized.");

  delay(100);

  // Create CSV file with headers if it doesn't exist
  if (!SD.exists(filename)) {
    Serial.println("File doesn't exist, creating new file...");
    File dataFile = SD.open(filename, FILE_WRITE);
    if (dataFile) {
      Serial.println("File opened successfully, writing headers...");
      dataFile.println("timestamp,ph,temperature,dissolved_oxygen,electrical_conductivity,turbidity_ntu,error_code");
      dataFile.close();
      Serial.println("Created new CSV file with headers");
    } else {
      Serial.println("Error creating CSV file");
    }
  } else {
    Serial.println("CSV file already exists");
  }
}

void loop() {
  writeDataToFile();
  delay(1000);
}

void writeDataToFile() {
  File dataFile = SD.open(filename, FILE_APPEND);

  if (dataFile) {

    // Write timestamp

    // Write sensor data

    //Time
    dataFile.print("Timestamp: (Month)"); //File
    dataFile.print(tm_mon);
    dataFile.print(", (Day)");
    dataFile.print(tm_mday);
    dataFile.print(", (Time)");
    dataFile.print(tm_hour);
    dataFile.print(":");
    dataFile.print(tm_min);
    dataFile.print(":");
    dataFile.println(tm_sec);

    Serial.print("Timestamp: (Month)"); //Serial
    Serial.print(tm_mon);
    Serial.print(", (Day)");
    Serial.print(tm_mday);
    Serial.print(", (Time)");
    Serial.print(tm_hour);
    Serial.print(":");
    Serial.print(tm_min);
    Serial.print(":");
    Serial.println(tm_sec);

    //Current
    dataFile.print("Current: "); //File
    dataFile.print(ina260.readCurrent());
    dataFile.println(" mA");

    Serial.print("Current: "); //Serial
    Serial.print(ina260.readCurrent());
    Serial.println(" mA");

    //Voltage
    dataFile.print("Voltage: "); //File
    dataFile.print(ina260.readBusVoltage());
    dataFile.println(" mV");

    Serial.print("Voltage: "); //Serial
    Serial.print(ina260.readBusVoltage());
    Serial.println(" mV");

    //Power
    dataFile.print("Power: "); //File
    dataFile.print(ina260.readPower());
    dataFile.println(" mW");

    Serial.print("Power: "); //Serial
    Serial.print(ina260.readPower());
    Serial.println(" mW");

    dataFile.println();

    dataFile.close();
    Serial.println("Data saved to SD card");

  } else {
    Serial.println("Error opening " + filename + " for writing");
  }
}
