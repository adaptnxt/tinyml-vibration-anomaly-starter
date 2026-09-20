/**
 * @file vibration_analyzer.h
 * @brief Embedded feature extraction and anomaly scoring engine.
 * @author AdaptNXT Technology Solutions
 */

#ifndef ADAPTNXT_VIBRATION_ANALYZER_H
#define ADAPTNXT_VIBRATION_ANALYZER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define VIBRATION_NUM_FEATURES 5

typedef struct {
    float rms;
    float peak_to_peak;
    float crest_factor;
    float kurtosis;
    float dominant_freq_hz;
} VibrationFeatures;

/**
 * @brief Computes statistical and spectral features from raw vibration buffer.
 * @param samples Raw acceleration buffer (e.g. Gs or m/s^2), length must be power of 2
 * @param count Number of samples (e.g. 128, 256)
 * @param sample_rate_hz Sampling rate in Hertz (e.g. 1000.0f)
 * @param out_features Pointer to output feature struct
 */
void extract_vibration_features(
    const float *samples,
    size_t count,
    float sample_rate_hz,
    VibrationFeatures *out_features
);

/**
 * @brief Evaluates normalized distance anomaly score against trained baseline.
 * @param features Computed features struct
 * @param means Array of baseline means (length VIBRATION_NUM_FEATURES)
 * @param stds Array of baseline standard deviations (length VIBRATION_NUM_FEATURES)
 * @param threshold Anomaly decision threshold (typically 2.5f to 3.5f)
 * @param out_score Pointer to receive calculated normalized anomaly score
 * @return true if anomalous, false if normal machine condition
 */
bool evaluate_anomaly(
    const VibrationFeatures *features,
    const float *means,
    const float *stds,
    float threshold,
    float *out_score
);

#ifdef __cplusplus
}
#endif

#endif /* ADAPTNXT_VIBRATION_ANALYZER_H */
