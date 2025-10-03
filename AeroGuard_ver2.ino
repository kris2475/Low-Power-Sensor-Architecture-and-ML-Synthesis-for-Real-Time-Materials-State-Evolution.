#include <SPI.h>
#include <SD.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include <Adafruit_PM25AQI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>

// --- Configuration Constants ---
const int WARMUP_MINUTES = 30;
const int LOG_PERIOD_SECONDS = 1;
const int LOGS_BEFORE_WRITE = 600;
const int SD_CS_PIN = 5;
const int I2C_SDA_PIN = 21;
const int I2C_SCL_PIN = 22;

// --- GPS Configuration ---
#define GPS_RX_PIN 16   // GPS TX -> ESP32 RX2
#define GPS_TX_PIN 17   // GPS RX -> ESP32 TX2
TinyGPSPlus gps;
HardwareSerial gpsSerial(2);  // Use UART2 for pins 16/17

// --- State Variables ---
unsigned long previousMillis = 0;
int logCounter = 0;
String logBuffer = "";
bool warmupComplete = false;
bool sdWriting = false;
bool sdWriteFailed = false;
unsigned long warmupEndTime;
unsigned long sdWriteResultDisplayTime = 0;
String logFileName = ""; // New variable to store the dynamic filename

// --- PMSA003I Sensor Configuration (Direct I2C) ---
#define PMSA003I_I2C_ADDR 0x12
Adafruit_PM25AQI pmsa;
struct PMSA_Data {
  uint16_t pm10_standard;
  uint16_t pm25_standard;
  uint16_t pm100_standard;
  uint16_t particles_03um;
  uint16_t particles_05um;
  uint16_t particles_10um;
  uint16_t particles_25um;
  uint16_t particles_50um;
  uint16_t particles_100um;
};
PMSA_Data currentPMData;
bool pmsaDataValid = false;

// --- BME680 Sensor Configuration ---
Adafruit_BME680 bme;
bool bme680DataValid = false;

// --- Fermion MEMS Gas Sensor Analog Pins ---
#define NO2_PIN 33
#define CO_PIN 34
#define H2S_PIN 35
float no2_voltage = 0.0;
float co_voltage = 0.0;
float h2s_voltage = 0.0;
esp_adc_cal_characteristics_t adc_chars;
bool adc_calibration_initialized = false;

// --- OLED Display Configuration ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3D
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- Function Prototypes ---
void setupSDCard();
void setupGPS();
void setupSensors();
void logData();
void updateDisplay();
bool readPMSA003I_I2C();
void readBME680AndGasSensors();

void setup() {
  Serial.begin(115200);
  Serial.println(F("AeroGuard starting up..."));

  setupSDCard();

  Serial.println(F("Initializing I2C bus and OLED display..."));
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(400000);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed. Check wiring and address."));
    while (1);
  }
  Serial.println(F("OLED initialized."));
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("AeroGuard starting...");
  display.display();
  delay(2000);

  setupSensors();
  setupGPS();

  warmupEndTime = millis() + (unsigned long)WARMUP_MINUTES * 60 * 1000UL;
  Serial.print(F("Starting BME680 warmup period: "));
  Serial.print(WARMUP_MINUTES);
  Serial.println(F(" minutes)..."));

  // Find the next available filename and create the file
  int fileIndex = 0;
  do {
    logFileName = "/gpslog" + String(fileIndex) + ".csv";
    fileIndex++;
  } while (SD.exists(logFileName));

  Serial.print(F("Creating new log file: "));
  Serial.println(logFileName);

  File logFile = SD.open(logFileName, FILE_WRITE);
  if (!logFile) {
    Serial.println(F("Failed to create and open log file."));
    while (1);
  } else {
    // Added column headers for CSV
    logFile.println("timestamp,latitude,longitude,altitude,satellites,speed,heading,hdop,pm1_standard,pm25_standard,pm10_standard,particles_03um,particles_05um,particles_10um,particles_25um,particles_50um,particles_100um,bme_temperature_c,bme_humidity_percent,bme_pressure_hpa,bme_gas_resistance_ohms,no2_voltage_v,co_voltage_v,h2s_voltage_v");
    logFile.close();
    Serial.println(F("New log file created and header written."));
  }
}

void loop() {
  // feed TinyGPS++ with raw GPS chars
  while (gpsSerial.available() > 0) {
    char c = gpsSerial.read();
    gps.encode(c);
  }

  if (!warmupComplete && millis() >= warmupEndTime) {
    warmupComplete = true;
    Serial.println(F("BME680 warmup period complete. Starting to log data."));
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= (unsigned long)LOG_PERIOD_SECONDS * 1000UL) {
    previousMillis = currentMillis;
    pmsaDataValid = readPMSA003I_I2C();
    readBME680AndGasSensors();
    updateDisplay();
    if (warmupComplete) logData();
  }
}

void setupSDCard() {
  Serial.print(F("Initializing SD card on CS="));
  Serial.println(SD_CS_PIN);
  if (!SD.begin(SD_CS_PIN, SPI, 25000000)) {
    Serial.println(F("SD card initialization failed!"));
    while (1);
  }
  Serial.println(F("SD card initialized successfully."));
}

void setupGPS() {
  Serial.println(F("Initializing GPS module..."));
  gpsSerial.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  Serial.println(F("GPS module initialized on pins 16/17."));
}

void setupSensors() {
  Serial.println("Initializing PMSA003I sensor (Direct I2C)...");
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    bme680DataValid = false;
  } else {
    Serial.println("BME680 found and initialized.");
    bme680DataValid = true;
  }
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150);

  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11);
  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);
  adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 0, &adc_chars);
  adc_calibration_initialized = (esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 0, &adc_chars) == ESP_OK);
}

void logData() {
  if (!gps.location.isValid() || !gps.date.isValid() || !gps.time.isValid()) {
    Serial.println(F("No valid GPS data. Not logging."));
    return;
  }

  String logEntry = "";
  // Formatted as YYYY-MM-DDTHH:MM:SS
  logEntry += String(gps.date.year());
  logEntry += "-";
  if (gps.date.month() < 10) logEntry += "0";
  logEntry += String(gps.date.month());
  logEntry += "-";
  if (gps.date.day() < 10) logEntry += "0";
  logEntry += String(gps.date.day());
  logEntry += "T";
  if (gps.time.hour() < 10) logEntry += "0";
  logEntry += String(gps.time.hour());
  logEntry += ":";
  if (gps.time.minute() < 10) logEntry += "0";
  logEntry += String(gps.time.minute());
  logEntry += ":";
  if (gps.time.second() < 10) logEntry += "0";
  logEntry += String(gps.time.second());

  logEntry += ",";
  logEntry += String(gps.location.lat(), 6);
  logEntry += ",";
  logEntry += String(gps.location.lng(), 6);
  logEntry += ",";
  logEntry += String(gps.altitude.meters());
  logEntry += ",";
  logEntry += String(gps.satellites.value());
  logEntry += ",";
  logEntry += String(gps.speed.kmph());
  logEntry += ",";
  logEntry += String(gps.course.deg());
  logEntry += ",";
  logEntry += String(gps.hdop.hdop());

  if (pmsaDataValid) {
    logEntry += ",";
    logEntry += String(currentPMData.pm10_standard);
    logEntry += ",";
    logEntry += String(currentPMData.pm25_standard);
    logEntry += ",";
    logEntry += String(currentPMData.pm100_standard);
    logEntry += ",";
    logEntry += String(currentPMData.particles_03um);
    logEntry += ",";
    logEntry += String(currentPMData.particles_05um);
    logEntry += ",";
    logEntry += String(currentPMData.particles_10um);
    logEntry += ",";
    logEntry += String(currentPMData.particles_25um);
    logEntry += ",";
    logEntry += String(currentPMData.particles_50um);
    logEntry += ",";
    logEntry += String(currentPMData.particles_100um);
  } else {
    logEntry += ",,,,,,,,,";
  }

  if (bme680DataValid) {
    logEntry += ",";
    logEntry += String(bme.temperature);
    logEntry += ",";
    logEntry += String(bme.humidity);
    logEntry += ",";
    logEntry += String(bme.pressure / 100.0F);
    logEntry += ",";
    logEntry += String(bme.gas_resistance);
  } else {
    logEntry += ",,,,";
  }

  logEntry += ",";
  logEntry += String(no2_voltage, 4);
  logEntry += ",";
  logEntry += String(co_voltage, 4);
  logEntry += ",";
  logEntry += String(h2s_voltage, 4);
  logBuffer += logEntry + "\n";

  logCounter++;
  Serial.print(F("Log buffered. Count: "));
  Serial.println(logCounter);

  if (logCounter >= LOGS_BEFORE_WRITE) {
    sdWriting = true;
    updateDisplay();

    Serial.println(F("Buffer full. Writing to SD card..."));
    // Open the file in append mode to avoid overwriting previous data.
    File logFile = SD.open(logFileName, FILE_APPEND);
    if (logFile) {
      logFile.print(logBuffer);
      logFile.close();
      Serial.println(F("Write complete."));
      sdWriteFailed = false;
    } else {
      Serial.println(F("Failed to open file for writing."));
      sdWriteFailed = true;
    }

    logBuffer = "";
    logCounter = 0;
    sdWriting = false;
    sdWriteResultDisplayTime = millis();
  }
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  if (millis() - sdWriteResultDisplayTime < 3000) {
    if (!sdWriteFailed) {
      display.println("Write complete.");
    } else {
      display.println("SD Write Failed!");
      display.println("Check card.");
    }
  } else if (sdWriting) {
    display.println("Writing to SD Card...");
    display.println("DO NOT DISCONNECT!");
  } else if (!warmupComplete) {
    long timeRemaining = (warmupEndTime - millis()) / 1000;
    if (timeRemaining > 0) {
      display.println("Warming up...");
      display.print("GPS: ");
      display.println(gps.location.isValid() ? "Fix" : "Searching...");
      display.print("Time left: ");
      display.print(timeRemaining / 60);
      display.println(" min");
    } else {
      display.println("Warmup complete!");
    }
  } else {
    // New display content when logging
    display.println("Logging data...");
    display.print("Buffer: ");
    display.print(logCounter);
    display.print("/");
    display.println(LOGS_BEFORE_WRITE);
    display.print("Lat: ");
    if (gps.location.isValid()) {
      display.println(gps.location.lat(), 6);
    } else {
      display.println("---");
    }

    display.print("Lon: ");
    if (gps.location.isValid()) {
      display.println(gps.location.lng(), 6);
    } else {
      display.println("---");
    }

    display.setCursor(0, 32); // Move cursor for second block
    display.print("Time: ");
    if (gps.time.isValid()) {
      char timeString[9]; // HH:MM:SS\0
      sprintf(timeString, "%02d:%02d:%02d", gps.time.hour(), gps.time.minute(), gps.time.second());
      display.println(timeString);
    } else {
      display.println("---");
    }
    
    display.print("PM2.5: ");
    if (pmsaDataValid) {
      display.println(currentPMData.pm25_standard);
    } else {
      display.println("NO DATA");
    }

    display.print("Temp: ");
    if (bme680DataValid) {
      display.print(bme.temperature);
      display.println(" C");
    } else {
      display.println("NO DATA");
    }
  }

  display.display();
}

bool readPMSA003I_I2C() {
  uint8_t data[32];
  memset(data, 0, sizeof(data));

  Wire.beginTransmission(PMSA003I_I2C_ADDR);
  Wire.endTransmission();
  Wire.requestFrom(PMSA003I_I2C_ADDR, (uint8_t)32);

  int i = 0;
  while (Wire.available() && i < 32) {
    data[i++] = Wire.read();
  }

  if (i == 32) {
    if (data[0] == 0x42 && data[1] == 0x4D) {
      uint16_t checksum = 0;
      for (int j = 0; j < 30; j++) {
        checksum += data[j];
      }
      uint16_t receivedChecksum = (data[30] << 8) | data[31];

      if (checksum == receivedChecksum) {
        currentPMData.pm10_standard = (data[4] << 8) | data[5];
        currentPMData.pm25_standard = (data[6] << 8) | data[7];
        currentPMData.pm100_standard = (data[8] << 8) | data[9];
        currentPMData.particles_03um = (data[10] << 8) | data[11];
        currentPMData.particles_05um = (data[12] << 8) | data[13];
        currentPMData.particles_10um = (data[14] << 8) | data[15];
        currentPMData.particles_25um = (data[16] << 8) | data[17];
        currentPMData.particles_50um = (data[18] << 8) | data[19];
        currentPMData.particles_100um = (data[20] << 8) | data[21];
        return true;
      }
    }
  }
  return false;
}

void readBME680AndGasSensors() {
  if (bme680DataValid) {
    if (!bme.performReading()) {
      Serial.println("BME680: Failed to perform reading.");
    }
  }

  const float VOLTAGE_DIVIDER_FACTOR = 2.0;
  uint32_t voltage_at_pin_mV;

  int rawNO2 = adc1_get_raw(ADC1_CHANNEL_5);
  if (adc_calibration_initialized) {
    voltage_at_pin_mV = esp_adc_cal_raw_to_voltage(rawNO2, &adc_chars);
    no2_voltage = (float)voltage_at_pin_mV / 1000.0 * VOLTAGE_DIVIDER_FACTOR;
  } else {
    no2_voltage = (rawNO2 / 4095.0) * 3.3 * VOLTAGE_DIVIDER_FACTOR;
  }

  int rawCO = adc1_get_raw(ADC1_CHANNEL_6);
  if (adc_calibration_initialized) {
    voltage_at_pin_mV = esp_adc_cal_raw_to_voltage(rawCO, &adc_chars);
    co_voltage = (float)voltage_at_pin_mV / 1000.0 * VOLTAGE_DIVIDER_FACTOR;
  } else {
    co_voltage = (rawCO / 4095.0) * 3.3 * VOLTAGE_DIVIDER_FACTOR;
  }

  int rawH2S = adc1_get_raw(ADC1_CHANNEL_7);
  if (adc_calibration_initialized) {
    voltage_at_pin_mV = esp_adc_cal_raw_to_voltage(rawH2S, &adc_chars);
    h2s_voltage = (float)voltage_at_pin_mV / 1000.0 * VOLTAGE_DIVIDER_FACTOR;
  } else {
    h2s_voltage = (rawH2S / 4095.0) * 3.3 * VOLTAGE_DIVIDER_FACTOR;
  }
}






























