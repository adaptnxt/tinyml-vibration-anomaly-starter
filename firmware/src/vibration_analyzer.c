/**
 * @file vibration_analyzer.c
 * @brief Zero-allocation statistical and spectral vibration analyzer.
 */

#include "vibration_analyzer.h"
#include "fft.h"
#include <math.h>
#include <string.h>

#define MAX_FFT_POINTS 512

void extract_vibration_features(
    const float *samples,
    size_t count,
    float sample_rate_hz,
    VibrationFeatures *out_features
) {
    if (!samples || !out_features || count == 0) return;

    float sum = 0.0f;
    float sum_sq = 0.0f;
    float min_val = samples[0];
    float max_val = samples[0];

    for (size_t i = 0; i < count; i++) {
        float val = samples[i];
        sum += val;
        sum_sq += val * val;
        if (val < min_val) min_val = val;
        if (val > max_val) max_val = val;
    }

    float mean = sum / (float)count;
    float rms = sqrtf(sum_sq / (float)count);
    float p2p = max_val - min_val;

    // Kurtosis calculation
    float sum_fourth = 0.0f;
    float variance = (sum_sq / (float)count) - (mean * mean);
    if (variance < 1e-7f) variance = 1e-7f;

    for (size_t i = 0; i < count; i++) {
        float diff = samples[i] - mean;
        sum_fourth += diff * diff * diff * diff;
    }
    float kurtosis = (sum_fourth / (float)count) / (variance * variance);
    float peak_abs = fabsf(max_val) > fabsf(min_val) ? fabsf(max_val) : fabsf(min_val);
    float crest_factor = (rms > 1e-6f) ? (peak_abs / rms) : 0.0f;

    // Spectral analysis via Radix-2 FFT
    float dominant_freq = 0.0f;
    if (count <= MAX_FFT_POINTS && (count & (count - 1)) == 0) {
        static float fft_real[MAX_FFT_POINTS];
        static float fft_imag[MAX_FFT_POINTS];
        static float fft_mag[MAX_FFT_POINTS / 2];

        memcpy(fft_real, samples, count * sizeof(float));
        memset(fft_imag, 0, count * sizeof(float));

        apply_hanning_window(fft_real, count);
        compute_radix2_fft(fft_real, fft_imag, count);
        compute_fft_magnitudes(fft_real, fft_imag, fft_mag, count);

        // Find dominant non-DC peak
        float max_mag = 0.0f;
        size_t max_bin = 1;
        size_t half_n = count / 2;

        for (size_t bin = 1; bin < half_n; bin++) {
            if (fft_mag[bin] > max_mag) {
                max_mag = fft_mag[bin];
                max_bin = bin;
            }
        }
        dominant_freq = ((float)max_bin * sample_rate_hz) / (float)count;
    }

    out_features->rms = rms;
    out_features->peak_to_peak = p2p;
    out_features->crest_factor = crest_factor;
    out_features->kurtosis = kurtosis;
    out_features->dominant_freq_hz = dominant_freq;
}

bool evaluate_anomaly(
    const VibrationFeatures *features,
    const float *means,
    const float *stds,
    float threshold,
    float *out_score
) {
    if (!features || !means || !stds) return false;

    float feat_arr[VIBRATION_NUM_FEATURES] = {
        features->rms,
        features->peak_to_peak,
        features->crest_factor,
        features->kurtosis,
        features->dominant_freq_hz
    };

    float distance_sq = 0.0f;
    for (size_t i = 0; i < VIBRATION_NUM_FEATURES; i++) {
        float std_dev = stds[i] > 1e-6f ? stds[i] : 1.0f;
        float z = (feat_arr[i] - means[i]) / std_dev;
        distance_sq += z * z;
    }

    float score = sqrtf(distance_sq / (float)VIBRATION_NUM_FEATURES);
    if (out_score) {
        *out_score = score;
    }

    return (score > threshold);
}
