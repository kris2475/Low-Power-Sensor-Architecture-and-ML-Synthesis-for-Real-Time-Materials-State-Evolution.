# 🧪 Polymer Curing Monitoring with ESP32, BME680, and EtOH/VOC/NH₃ Sensors

## Project Overview
This project uses an **ESP32 microcontroller** with a **BME680** and **EtOH, VOC, and NH₃ sensors** to monitor the curing state of polymers such as **epoxy** and **silicone**.  

By analyzing the **volatile organic compounds (VOCs)** and amine emissions produced during curing, combined with environmental context from the BME680, the system can **non-destructively classify the material’s curing stage in real-time**.

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
- **Test Chamber**: Small, sealed container (e.g., 500 mL) to accumulate VOCs and amine emissions.  
- **Power Supply**: Stable 5V for sensors and ESP32.  

**Wiring Notes:**
- Each analog sensor (EtOH, VOC, NH₃) connects to an ESP32 analog input.  
- BME680 communicates over I2C (SDA/SCL).  
- Take baseline readings in **clean air** before starting experiments.

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

---

## References
- VOC and amine emissions during epoxy and silicone curing: chemical kinetics literature.  
- EtOH, VOC, and NH₃ sensor datasheets for voltage-to-gas response.  
- BME680 environmental sensing and gas resistance correction methodology.  
- M-LENS methodology: Materials-informed Localized Electronic Nose System.





