# Changelog

All notable changes to this project will be documented in this file.

## [0.1.0] - 2026-09-20
### Added
- Initial open-source release of AdaptNXT TinyML Vibration Anomaly Starter.
- High-performance, zero-allocation C99 Radix-2 Fast Fourier Transform (FFT) engine.
- Time and frequency domain statistical feature extraction (RMS, Peak-to-Peak, Kurtosis, Crest Factor, Dominant Frequency).
- One-Class Anomaly scoring model with C-header generation (`model_data.h`) for ESP32 and Cortex-M.
- Synthetic bearing vibration data generator (normal baseline, outer race bearing fault, imbalance).
- Unit test suite verifying FFT spectral peaks and anomaly detection accuracy.
- GCC CI pipeline testing compilation of embedded C firmware.
