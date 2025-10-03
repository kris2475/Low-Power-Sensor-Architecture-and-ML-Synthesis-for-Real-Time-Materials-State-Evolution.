# AeroGuard Environmental Data Logger

This Arduino/ESP32 sketch implements a **portable environmental data logger** that collects GPS, air quality, and gas sensor data, displays real-time information on an OLED, and logs it to an SD card.

---

## Table of Contents

- [Features](#features)
- [Hardware Interfaces](#hardware-interfaces)
- [Configuration Constants](#configuration-constants)
- [Setup Sequence](#setup-sequence)
- [Main Loop](#main-loop)
- [Logging](#logging)
- [OLED Display](#oled-display)
- [Sensor Reading Functions](#sensor-reading-functions)
- [Special Features](#special-features)

---

## Features

- GPS location tracking
- Particulate matter measurement (PM1.0, PM2.5, PM10, particle counts)
- Temperature, humidity, pressure, and gas resistance (BME680)
- NO₂, CO, and H₂S gas voltage readings
- Real-time display on OLED
- CSV logging to SD card with buffered writes

---

## Hardware Interfaces

| Component | Interface | Pins |
|-----------|----------|------|
| GPS Module | UART2 | RX=16, TX=17 |
| PMSA003I PM Sensor | I2C | Address=0x12 |
| BME680 Sensor | I2C | Address=0x76 |
| Gas Sensors (NO₂, CO, H₂S) | ADC | 33, 34, 35 |
| OLED Display | I2C | SDA=21, SCL=22 |
| SD Card | SPI | CS=5 |

---

## Configuration Constants

| Constant | Value | Purpose |
|----------|-------|---------|
| `WARMUP_MINUTES` | 30 | BME680 warmup period before logging |
| `LOG_PERIOD_SECONDS` | 1 | Data collection interval |
| `LOGS_BEFORE_WRITE` | 600 | Buffer logs in memory before writing to SD |
| `PMSA003I_I2C_ADDR` | 0x12 | I2C address of particulate sensor |

---

## Setup Sequence

1. Initialize serial debugging.
2. Initialize **SD card**.
3. Initialize **I2C bus** and OLED display.
4. Initialize **sensors**:
   - BME680 (temperature, humidity, pressure, gas resistance)
   - ADC channels for NO₂, CO, H₂S sensors
5. Initialize **GPS module** on UART2.
6. Determine next available log filename (e.g., `/gpslog0.csv`) and write CSV headers.

---

## Main Loop (`loop()`)

- **GPS data**: Continuously feed GPS characters to `TinyGPS++`.
- **Warmup handling**: Wait until BME680 warmup (30 minutes) is complete.
- **Periodic logging** (every second):
  1. Read PMSA003I particulate sensor via I2C.
  2. Read BME680 and gas sensor voltages.
  3. Update OLED display with current data.
  4. Log data to memory buffer if warmup is complete.

---

## Logging (`logData()`)

- Checks GPS validity.
- Constructs a CSV entry including:
  - **Timestamp** (`YYYY-MM-DDTHH:MM:SS`)
  - **GPS**: latitude, longitude, altitude, speed, heading, satellites, HDOP
  - **PM Sensor**: PM1.0, PM2.5, PM10, particle counts
  - **BME680**: temperature, humidity, pressure, gas resistance
  - **Gas sensors**: NO₂, CO, H₂S voltages
- Appends entry to memory buffer.
- Writes buffer to SD card after reaching 600 entries.

---

## OLED Display (`updateDisplay()`)

Shows different statuses:

- **Warmup**: Time remaining, GPS status.
- **Logging**: Current GPS location, PM2.5, temperature, buffer status.
- **SD card write**: Shows "Writing to SD Card" or success/failure messages.

---

## Sensor Reading Functions

- `readPMSA003I_I2C()` → Reads 32-byte frame from PMSA003I, verifies checksum, updates PM values.
- `readBME680AndGasSensors()` → Reads BME680 and analog gas sensors, converts ADC raw values to voltages with calibration.

---

## Special Features

- **Buffered logging** reduces SD card wear.
- **Real-time OLED feedback** for monitoring.
- **Automatic filename increment** prevents overwriting previous logs.
- **Warmup logic** ensures accurate BME680 gas readings.

---

## Usage

1. Connect the sensors, GPS, OLED, and SD card module to the ESP32.
2. Upload the sketch.
3. Monitor startup and warmup on the OLED.
4. Data is logged to SD card in CSV format once the BME680 warmup is complete.
5. Retrieve log files for analysis.

---

## CSV Output Example

```csv
timestamp,latitude,longitude,altitude,satellites,speed,heading,hdop,pm1_standard,pm25_standard,pm10_standard,particles_03um,particles_05um,particles_10um,particles_25um,particles_50um,particles_100um,bme_temperature_c,bme_humidity_percent,bme_pressure_hpa,bme_gas_resistance_ohms,no2_voltage_v,co_voltage_v,h2s_voltage_v
2025-10-03T12:00:01,37.774929,-122.419416,12,7,0.0,0.0,0.9,5,12,15,120,80,40,20,10,5,22.5,55.2,1013.2,1200,0.4000,0.3200,0.2500
