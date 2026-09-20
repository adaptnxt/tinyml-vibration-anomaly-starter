/**
 * @file main.c
 * @brief Demonstration runner for TinyML vibration anomaly detection.
 */

#include <stdio.h>
#include <math.h>
#include "vibration_analyzer.h"
#include "model_data.h"

#define SAMPLE_COUNT 128
#define SAMPLE_RATE_HZ 1000.0f

int main(void) {
    printf("=====================================================\n");
    printf(" AdaptNXT TinyML Vibration Sentinel (Firmware Demo)\n");
    printf(" Model Version: %s | Threshold: %.2f\n", VIBRATION_MODEL_VERSION, VIBRATION_THRESHOLD);
    printf("=====================================================\n\n");

    // 1. Simulate Normal Baseline Signal (60 Hz sine wave + small noise)
    float normal_buffer[SAMPLE_COUNT];
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        float t = (float)i / SAMPLE_RATE_HZ;
        normal_buffer[i] = sinf(2.0f * 3.14159f * 60.0f * t);
    }

    VibrationFeatures normal_features;
    extract_vibration_features(normal_buffer, SAMPLE_COUNT, SAMPLE_RATE_HZ, &normal_features);

    float normal_score = 0.0f;
    bool is_normal_anomalous = evaluate_anomaly(
        &normal_features,
        VIBRATION_BASELINE_MEANS,
        VIBRATION_BASELINE_STDS,
        VIBRATION_THRESHOLD,
        &normal_score
    );

    printf("[TEST 1: Normal Operation]\n");
    printf("  RMS: %.3f G | P2P: %.3f G | Kurtosis: %.2f | Dom Freq: %.1f Hz\n",
           normal_features.rms, normal_features.peak_to_peak,
           normal_features.kurtosis, normal_features.dominant_freq_hz);
    printf("  Anomaly Score: %.2f -> Status: %s\n\n",
           normal_score, is_normal_anomalous ? "ANOMALY DETECTED" : "HEALTHY (PASSED)");

    // 2. Simulate Bearing Fault Signal (High peak shocks + harmonic shift to 180 Hz)
    float fault_buffer[SAMPLE_COUNT];
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        float t = (float)i / SAMPLE_RATE_HZ;
        float base = sinf(2.0f * 3.14159f * 60.0f * t);
        float impact = ((i % 16) == 0) ? 3.5f : 0.0f; // Periodic mechanical impacts
        fault_buffer[i] = base + impact;
    }

    VibrationFeatures fault_features;
    extract_vibration_features(fault_buffer, SAMPLE_COUNT, SAMPLE_RATE_HZ, &fault_features);

    float fault_score = 0.0f;
    bool is_fault_anomalous = evaluate_anomaly(
        &fault_features,
        VIBRATION_BASELINE_MEANS,
        VIBRATION_BASELINE_STDS,
        VIBRATION_THRESHOLD,
        &fault_score
    );

    printf("[TEST 2: Bearing Impact Fault]\n");
    printf("  RMS: %.3f G | P2P: %.3f G | Kurtosis: %.2f | Dom Freq: %.1f Hz\n",
           fault_features.rms, fault_features.peak_to_peak,
           fault_features.kurtosis, fault_features.dominant_freq_hz);
    printf("  Anomaly Score: %.2f -> Status: %s\n\n",
           fault_score, is_fault_anomalous ? "ANOMALY DETECTED (TRIGGERED ALARM)" : "HEALTHY");

    if (!is_normal_anomalous && is_fault_anomalous) {
        printf(">> Firmware Sentinel Verification: ALL CHECKS PASSED.\n");
        return 0;
    } else {
        printf(">> Firmware Sentinel Verification: FAILED.\n");
        return 1;
    }
}
