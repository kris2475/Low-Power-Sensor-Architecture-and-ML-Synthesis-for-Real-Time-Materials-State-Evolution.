# 🧪 Polymer Curing Monitoring with ESP32, BME680, and Fermion Sensors

## Project Overview
This project uses an **ESP32 microcontroller** with a **BME680** and a **three-sensor Fermion MEMS array** to monitor the curing state of polymers such as **epoxy** and **silicone**.  

By analysing the **volatile organic compounds (VOCs)** emitted during curing, combined with environmental context from the BME680, the system can **non-destructively classify the material’s stage in real-time**.

---

## Selected Sensor Array

| Sensor | Target | Role |
|--------|--------|------|
| Alcohol/Ketone (Fermion) | Ethanol, acetone | Detects VOCs during initial curing phase |
| Aldehyde / HCHO (Fermion) | Formaldehyde, amines | Detects gelling/transition phase emissions |
| VOC / MQ-135 type (Fermion) | Miscellaneous organics | Completes multi-dimensional chemical fingerprint |
| BME680 | Temperature, humidity, pressure, gas resistance | Provides environmental context and baseline VOC trend |

> The combination creates a **distinctive chemical fingerprint** suitable for machine learning classification of curing stages.

---

## Hardware Setup
- **ESP32 Microcontroller**: Reads analog sensor outputs and BME680 I2C data, logs data to SD card or streams via Wi-Fi.  
- **Test Chamber**: Small, sealed container (e.g., 500 mL) to accumulate VOCs.  
- **Power Supply**: Stable 5V for sensors and ESP32.  

**Wiring Notes:**
- Each Fermion sensor connects to an analog input on the ESP32.  
- BME680 communicates over I2C (SDA/SCL).  
- Take baseline readings in **clean air** before starting experiments.

---

## Software and Data Pipeline

1. **Sensor Reading**
   - Acquire **analog voltages** from Fermion sensors every 1 second.  
   - Read **BME680** for temperature, humidity, pressure, and gas resistance.  
   - Convert analog values to **normalized resistance ratios** (Rₛ/R₀) or scaled voltages.

2. **Data Preprocessing**
   - Normalise sensor readings to remove baseline drift.  
   - Optional: moving average or exponential smoothing to reduce noise.

3. **Machine Learning Classification**
   - Multi-class task: `[Uncured, Transition, Fully Cured, Degrading]`.  
   - Suggested algorithms:
     - **Support Vector Machine (SVM)** or **Random Forest** for small datasets.  
     - **Feed-Forward Neural Network (FNN)** for complex non-linear patterns.  

4. **Output**
   - Real-time OLED display or logging to SD card.  
   - Optional alerts when material reaches a specific curing stage.

---

## Usage Notes
- Allow **sensor warm-up** (~30 minutes for BME680).  
- Keep the **chamber sealed** to concentrate VOCs.  
- Calibrate baseline readings in clean air before experiments.  
- For ML models, **collect sufficient data** across all curing stages for accurate classification.

---

## References
- VOC emissions during epoxy and silicone curing: chemical kinetics literature.  
- Fermion sensor datasheets for voltage-to-gas response.  
- M-LENS methodology: Materials-informed Localised Electronic Nose System.




