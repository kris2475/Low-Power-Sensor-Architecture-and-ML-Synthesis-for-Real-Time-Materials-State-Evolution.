# 🧪 Materials Engineering and Machine Learning Synthesis for Non-Destructive Chemical State Assessment (M-LENS)

## Project Overview
**M-LENS (Materials-informed Localized Electronic Nose System)** is an interdisciplinary project that bridges **Materials Engineering**, **low-cost Electronics**, and **Machine Learning (ML)** to perform non-destructive, real-time chemical state monitoring of materials.

The goal is to demonstrate that the curing state or early degradation of a polymeric material can be accurately classified by analyzing the unique chemical "fingerprint" of its volatile organic compound (VOC) emissions using a simple array of **Metal-Oxide Semiconductor (MOS)** gas sensors (an "Electronic Nose").

This project showcases expertise in:
- **Materials Characterization**: Understanding material kinetics and chemical pathways that lead to VOC release.
- **Low-Cost Electronics**: Interfacing analog sensors with digital microcontrollers for reliable data acquisition.
- **Machine Learning/Deep Learning**: Implementing predictive classification models for multi-dimensional sensor data.

---

## 1. Materials Engineering Principle
The foundation of this project relies on **Material Curing Kinetics** and **Chemical Off-Gassing**.

When a thermoset polymer (like epoxy or silicone) cures, it undergoes a chemical reaction (polymerization and cross-linking) that converts it from a liquid to a solid. During this process, or as it begins to degrade, it releases specific VOCs.

- **The Signature**: The concentration and composition of these VOCs change predictably as the reaction progresses. An array of non-specific MOS sensors provides a unique multi-dimensional signal (a "chemical fingerprint") at each stage of the material's life.  
- **The Metric (Feature)**: The primary feature extracted from the sensors is the normalized resistance ratio:

\[
R_S / R_0
\]

Where:  
- \(R_S\): Sensor resistance in the presence of the Material Under Test (MUT).  
- \(R_0\): Sensor resistance in clean air (baseline).  

By tracking the vector of \(R_S / R_0\) values across the sensor array over time, we capture the transition of the material's chemical state.

---

## 2. Hardware and Electronics Setup
The sensor array and data logger are designed for **simplicity and low power consumption** (although the ML model is run offline in this demonstration).

### Required Components

| Component        | Purpose                      | Notes                                                                 |
|------------------|------------------------------|----------------------------------------------------------------------|
| **Microcontroller** | Data acquisition and logging. | ESP32 or similar with Wi-Fi capability for easy data streaming.       |
| **Sensor Array (E-Nose)** | Chemical "fingerprinting." | 3–4 distinct MOS gas sensors (e.g., MQ-2, MQ-3, MQ-7, MQ-135).        |
| **Test Chamber** | Controlled environment for VOC concentration. | Small, sealed plastic container (e.g., 500 mL volume).                 |
| **Material Under Test (MUT)** | The material being monitored. | Two-part epoxy resin or common silicone sealant.                       |
| **Power Supply** | Powering microcontroller & sensors. | Reliable 5 V source.                                                   |

---

### Data Acquisition Flow
1. Each MOS sensor is connected to an **analog input (ADC)** pin on the ESP32 via a simple voltage divider.  
2. The ESP32 reads the voltage \(V_{OUT}\) across the load resistor, which allows calculation of the sensor’s internal resistance \(R_S\).  
3. The microcontroller logs the \(R_S\) values for all sensors every \(T_{sample}\) seconds, along with timestamps.  
4. Data is streamed wirelessly via **Wi-Fi** to a host PC, or written to an **SD card** for offline analysis.  

---

## 3. Data Acquisition & ML/DL Pipeline

### 3.1 Data Collection Strategy
The system requires **labeled data** representing different material states:

| State Label | Description              | Data Collection Method |
|-------------|--------------------------|-------------------------|
| **State 1: Liquid/Uncured** | Initial mixing phase. | Log data for the first hour post-mixing. |
| **State 2: Gelling/Transition** | Viscosity rapidly increasing. | Log data between 1–5 hours post-mixing. |
| **State 3: Fully Cured/Stable** | Final solid state (Endpoint). | Log data after 24 hours, continuing for several days. |
| **State 4: Degradation** | Simulated failure state. | Expose a fully cured sample to mild heat/chemical stress and log the new fingerprint. |

---

### 3.2 Machine Learning Implementation
The objective is a **Multi-Class Classification task**:  

> Given a sensor vector  
> \[
> X = [R_S/R_0 (MQ2), R_S/R_0 (MQ3), ...]
> \]  
> Predict the corresponding **State Label**.

#### Toolkit
- **Language**: Python  
- **Libraries**: `pandas`, `numpy`, `scikit-learn` (data preprocessing & training), `matplotlib` (visualization), `TensorFlow/Keras` (optional deep learning).  

#### Model Selection
- **Preprocessing**: Normalize input features (e.g., `StandardScaler`).  
- **Model 1 (Baseline)**:  
  - Support Vector Machine (SVM)  
  - Random Forest Classifier  
  (Well-suited for small, structured datasets).  
- **Model 2 (Deep Learning Showcase)**:  
  - Feed-Forward Neural Network (FNN)  
  - Learns **non-linear decision boundaries** between material states in multi-dimensional sensor space.  

---

