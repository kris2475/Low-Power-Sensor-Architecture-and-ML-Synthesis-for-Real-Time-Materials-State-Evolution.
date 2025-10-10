# 🧪 Polymer Curing Monitoring with ESP32, BME680, and EtOH/VOC/NH₃ Sensors

## Project Overview
This project uses an **ESP32 microcontroller** with a **BME680** and **EtOH, VOC, and NH₃ sensors** to monitor the curing state of polymers such as **epoxy** and **silicone**.  

By analyzing the **volatile organic compounds (VOCs)** and amine emissions produced during curing, combined with environmental context from the BME680, the system can **non-destructively classify the material’s curing stage in real-time**.

**Experimental workflow includes:**
1. **24-hour burn-in** of the sensor system (all data logged).  
2. **24-hour baseline measurement** in clean air (all data logged).  
3. **24-hour cure monitoring** of **RS Pro Structural Epoxy Adhesive, part number 918-1401** (all data logged).  

All tests are conducted at **ambient room temperature**, which is recorded alongside sensor data.  
**All runs were carried out in a sealed plastic container** to concentrate VOCs and ammonia emissions.

---

## Selected Sensor Array

| Sensor | Target | Role |
|--------|--------|------|
| EtOH | Ethanol and small alcohols | Detects VOCs during the early curing phase |
| VOC | Broad spectrum organics | Provides general VOC fingerprint across all curing stages |
| NH₃ | Ammonia / amines | Detects emissions associated with gelation and chemical transition |
| BME680 | Temperature, humidity, pressure, gas resistance | Provides environmental context and baseline VOC trends |

> Together, these sensors create a **distinctive chemical fingerprint** suitable for machine learning classification of curing stages.

---

## Hardware Setup
- **ESP32 Microcontroller**: Reads analog sensor outputs and BME680 I2C data; logs data to SD card or streams via Wi-Fi.  
- **Test Chamber**: **Sealed plastic container** (e.g., 500 mL) to accumulate VOCs and amine emissions.  
- **Power Supply**: Stable 5V for sensors and ESP32.  

**Wiring Notes:**
- Each analog sensor (EtOH, VOC, NH₃) connects to an ESP32 analog input.  
- BME680 communicates over I2C (SDA/SCL).  
- Baseline readings should be collected in **clean air** before starting experiments.  

---

## Experimental Procedure

1. **24-Hour Burn-In**
   - Run all sensors continuously for 24 hours to stabilize responses.  
   - Log all sensor readings and environmental data.  

2. **24-Hour Baseline**
   - Record sensor readings in **clean air** for 24 hours to establish baseline levels.  
   - All data, including ambient room temperature, should be logged.  

3. **24-Hour Cure Monitoring**
   - Apply **RS Pro Structural Epoxy Adhesive, part number 918-1401** in the **sealed plastic container**.  
   - Monitor VOC, EtOH, NH₃, and BME680 environmental readings continuously.  
   - Log all data to capture the full curing profile.  

---

## Software and Data Pipeline

1. **Sensor Reading**
   - Acquire **analog voltages** from EtOH, VOC, and NH₃ sensors every 1 second.  
   - Read **BME680** for temperature, humidity, pressure, and gas resistance.  
   - Convert analog readings to **normalized resistance ratios** (Rₛ/R₀) or scaled voltages.

2. **Data Preprocessing**
   - Normalize sensor readings to remove baseline drift.  
   - Optional: apply moving average or exponential smoothing to reduce noise.

3. **Machine Learning Classification**
   - Multi-class task: `[Uncured, Transition, Fully Cured, Degrading]`.  
   - Suggested algorithms:
     - **Random Forest** or **Support Vector Machine (SVM)** for small datasets.  
     - **Feed-Forward Neural Network (FNN)** for complex non-linear patterns.  

4. **Output**
   - Real-time OLED display or logging to SD card.  
   - Optional alerts when material reaches a specific curing stage.

---

## Usage Notes
- Allow **sensor warm-up** (~30 minutes for BME680 and MOX sensors).  
- Keep the **chamber sealed** to concentrate VOCs and ammonia emissions.  
- Calibrate baseline readings in clean air before experiments.  
- For ML models, **collect sufficient labeled data** across all curing stages for accurate classification.  
- Ensure **ambient temperature logging** during all burn-in, baseline, and cure phases.

---

## References
- VOC and amine emissions during epoxy and silicone curing: chemical kinetics literature.  
- EtOH, VOC, and NH₃ sensor datasheets for voltage-to-gas response.  
- BME680 environmental sensing and gas resistance correction methodology.  
- M-LENS methodology: Materials-informed Localized Electronic Nose System.







