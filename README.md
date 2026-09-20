<div align="center">

# TinyML Vibration Anomaly Starter

### Real-Time FFT Anomaly Detection Firmware & Training Pipeline for Microcontrollers (&lt; 256KB RAM)

[![CI](https://github.com/adaptnxt/tinyml-vibration-anomaly-starter/actions/workflows/ci.yml/badge.svg)](https://github.com/adaptnxt/tinyml-vibration-anomaly-starter/actions/workflows/ci.yml)
[![License](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](LICENSE)
[![C Standard](https://img.shields.io/badge/C-C99%20Compliant-brightgreen.svg)](https://en.wikipedia.org/wiki/C99)
[![Python](https://img.shields.io/badge/Python-3.10%20%7C%203.11%20%7C%203.12%20%7C%203.13-green.svg)](https://www.python.org/)
[![Maintained by AdaptNXT](https://img.shields.io/badge/Maintained%20by-AdaptNXT-orange.svg)](https://www.adaptnxt.com)

<p align="center">
  A production-ready reference implementation for low-power edge vibration anomaly detection, real-time Radix-2 FFT spectral analysis, and zero-allocation statistical scoring designed for ESP32, ARM Cortex-M4/M7, and STM32 microcontrollers.
</p>

</div>

---

## 🏗️ Architecture Overview

```text
  3-Axis Accelerometer
 (ADXL345 / LIS3DH / MPU6050)
              │
              │ I2C / SPI DMA (1000 Hz)
              ▼
  ┌─────────────────────────────────────────────────────────────┐
  │                 Sample Circular Ring Buffer                 │
  │           (128 / 256 samples, float32, in SRAM)             │
  └──────────────────────────────┬──────────────────────────────┘
                                 │
                                 ▼
  ┌─────────────────────────────────────────────────────────────┐
  │         Feature Extraction & Spectral Analysis Engine       │
  │   • In-Place Hanning Windowing                              │
  │   • Zero-Allocation Radix-2 Decimation-In-Time FFT          │
  │   • Spectral Peak & Dominant Harmonic Extraction            │
  │   • Time-Domain: RMS, Peak-to-Peak, Kurtosis, Crest Factor  │
  └──────────────────────────────┬──────────────────────────────┘
                                 │ 5-Dimensional Feature Vector
                                 ▼
  ┌─────────────────────────────────────────────────────────────┐
  │             One-Class Anomaly Distance Evaluator            │
  │       • Normalized Z-Score Euclidean / Mahalanobis          │
  │       • Trained Calibration Baseline (model_data.h)         │
  └──────────────────────────────┬──────────────────────────────┘
                                 │
                 ┌───────────────┴───────────────┐
                 ▼                               ▼
       [Score <= Threshold]            [Score > Threshold]
         Normal Condition           🚨 Mechanical Fault Alert
          (Green LED / Log)          (Trigger Relay / Modbus / MQTT)
```

---

## 📊 Resource Footprint & Hardware Compatibility

Tested and optimized for resource-constrained industrial microcontrollers:

| Parameter | Specification | Notes |
| :--- | :--- | :--- |
| **RAM Consumption** | **~5 to 8 KB** | Static buffers (256-point FFT float arrays). Zero dynamic `malloc()` / `free()`. |
| **Flash / Code Size** | **< 16 KB** | Compiled with standard `-Os` or `-O2` optimization |
| **Inference Latency** | **~5-15 ms** (Typical 240MHz MCU) | FFT computation + feature extraction + statistical distance scoring |
| **Target MCUs** | ESP32, STM32F4, nRF52840, SAMD51 | Compatible with FreeRTOS, ESP-IDF, STM32 HAL, Arduino |

---

## ✨ Features

- **Zero-Allocation Pure C99**: Eliminates heap fragmentation in 24/7 continuous industrial environments.
- **High-Performance In-Place Radix-2 FFT**: Optimized decimation-in-time algorithm with bit-reversal and Hanning window multiplier.
- **5-Dimensional Statistical Profiling**:
  - `RMS`: Overall vibration energy (ISO 10816 machine vibration severity).
  - `Peak-to-Peak`: Total displacement shock magnitude.
  - `Kurtosis`: Early indicator of bearing spalling and impact pitting.
  - `Crest Factor`: Sharpness of mechanical impacts.
  - `Dominant Frequency (Hz)`: Rotational imbalance vs gear-mesh harmonics.
- **Python Training & Calibration Pipeline**: Generates synthetic baseline signals, simulates bearing outer race faults, and automatically exports embedded C header files (`model_data.h`).

---

## 🚀 Quick Start (Python Pipeline)

### 1. Installation

```bash
git clone https://github.com/adaptnxt/tinyml-vibration-anomaly-starter.git
cd tinyml-vibration-anomaly-starter
pip install -e ".[dev]"
```

### 2. Simulate Machine Anomalies via CLI

#### Simulate Normal Baseline Motor:
```bash
adaptnxt-tinyml simulate --fault none
```
*Output:*
```text
=======================================================
 AdaptNXT TinyML Vibration Sentinel Simulation
 Condition:       Healthy Baseline
 Buffer Size:     256 samples (1000 Hz)
-------------------------------------------------------
 RMS:             0.718 G
 Peak-to-Peak:    2.052 G
 Crest Factor:    1.43
 Kurtosis:        3.02
 Dominant Freq:   60.0 Hz
-------------------------------------------------------
 Anomaly Score:   0.24 (Threshold: 2.50)
 Status:          ✅ NORMAL (PASSED)
=======================================================
```

#### Simulate Bearing Impact Fault:
```bash
adaptnxt-tinyml simulate --fault bearing
```
*Output:*
```text
=======================================================
 AdaptNXT TinyML Vibration Sentinel Simulation
 Condition:       Bearing Outer Race Fault
 Buffer Size:     256 samples (1000 Hz)
-------------------------------------------------------
 RMS:             1.242 G
 Peak-to-Peak:    4.821 G
 Crest Factor:    3.88
 Kurtosis:        8.45
 Dominant Freq:   60.0 Hz
-------------------------------------------------------
 Anomaly Score:   8.92 (Threshold: 2.50)
 Status:          🚨 ANOMALY DETECTED
=======================================================
```

### 3. Calibrate & Export Embedded C Header

Generate a calibrated `model_data.h` containing your baseline machine weights:
```bash
adaptnxt-tinyml export-header --output firmware/include/model_data.h
```

---

## 🛠️ Embedded C Integration Guide

### 1. Include in your Firmware Project

Copy `firmware/include/` and `firmware/src/` into your ESP-IDF, STM32CubeIDE, or Arduino project.

```c
#include "vibration_analyzer.h"
#include "model_data.h"

#define BUFFER_SIZE 256
#define SAMPLE_RATE 1000.0f

float sensor_buffer[BUFFER_SIZE]; // Populated by ADC or I2C/SPI accelerometer

void on_buffer_ready() {
    VibrationFeatures features;
    extract_vibration_features(sensor_buffer, BUFFER_SIZE, SAMPLE_RATE, &features);

    float score = 0.0f;
    bool is_anomaly = evaluate_anomaly(
        &features,
        VIBRATION_BASELINE_MEANS,
        VIBRATION_BASELINE_STDS,
        VIBRATION_THRESHOLD,
        &score
    );

    if (is_anomaly) {
        gpio_set_level(ALARM_RELAY_PIN, 1);
        printf("[CRITICAL]: Vibration Anomaly! Score: %.2f\n", score);
    }
}
```

---

## 🧪 Testing

Run Python automated test suite:
```bash
pytest tests/ -v
```

Compile and run C firmware verification using GCC:
```bash
gcc -Wall -Wextra -std=c99 -I firmware/include \
    firmware/src/fft.c firmware/src/vibration_analyzer.c firmware/main.c \
    -lm -o vibration_sentinel
./vibration_sentinel
```

---

## 🏢 Enterprise Citation & Custom Hardware Engineering

Maintained by the Embedded Systems & TinyML engineering team at **[AdaptNXT Technology Solutions](https://www.adaptnxt.com)**.

For custom Edge AI firmware development, high-frequency piezoelectric sensor integration, and industrial gateway design:
* [TinyML & Microcontroller Edge AI Services](https://www.adaptnxt.com/tinyml-edge-ai-microcontroller-services)
* [Industrial IoT Platform Services](https://www.adaptnxt.com/service-industrial-iot-platform)
* [IoT FOTA & Fleet Device Management](https://www.adaptnxt.com/iot-fota-fleet-management-services)

---

## 📄 License

This project is licensed under the [Apache-2.0 License](LICENSE).
