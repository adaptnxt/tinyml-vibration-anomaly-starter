/**
 * @file fft.c
 * @brief High-performance embedded Radix-2 FFT implementation.
 */

#include "fft.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void apply_hanning_window(float *data, size_t length) {
    if (!data || length == 0) return;
    for (size_t i = 0; i < length; i++) {
        float multiplier = 0.5f * (1.0f - cosf((2.0f * (float)M_PI * (float)i) / (float)(length - 1)));
        data[i] *= multiplier;
    }
}

void compute_radix2_fft(float *real, float *imag, size_t n) {
    if (!real || !imag || n < 2) return;

    // Bit reversal permutation
    size_t j = 0;
    for (size_t i = 0; i < n - 1; i++) {
        if (i < j) {
            float temp_r = real[i];
            float temp_i = imag[i];
            real[i] = real[j];
            imag[i] = imag[j];
            real[j] = temp_r;
            imag[j] = temp_i;
        }
        size_t k = n >> 1;
        while (k <= j) {
            j -= k;
            k >>= 1;
        }
        j += k;
    }

    // Cooley-Tukey Radix-2 decimation-in-time
    for (size_t len = 2; len <= n; len <<= 1) {
        float angle = -2.0f * (float)M_PI / (float)len;
        float wlen_r = cosf(angle);
        float wlen_i = sinf(angle);

        for (size_t i = 0; i < n; i += len) {
            float w_r = 1.0f;
            float w_i = 0.0f;
            size_t half_len = len >> 1;

            for (size_t k = 0; k < half_len; k++) {
                size_t u_idx = i + k;
                size_t v_idx = i + k + half_len;

                float u_r = real[u_idx];
                float u_i = imag[u_idx];

                float v_r = real[v_idx] * w_r - imag[v_idx] * w_i;
                float v_i = real[v_idx] * w_i + imag[v_idx] * w_r;

                real[u_idx] = u_r + v_r;
                imag[u_idx] = u_i + v_i;
                real[v_idx] = u_r - v_r;
                imag[v_idx] = u_i - v_i;

                float next_w_r = w_r * wlen_r - w_i * wlen_i;
                float next_w_i = w_r * wlen_i + w_i * wlen_r;
                w_r = next_w_r;
                w_i = next_w_i;
            }
        }
    }
}

void compute_fft_magnitudes(const float *real, const float *imag, float *magnitudes, size_t n) {
    if (!real || !imag || !magnitudes) return;
    size_t half_n = n / 2;
    for (size_t i = 0; i < half_n; i++) {
        magnitudes[i] = sqrtf(real[i] * real[i] + imag[i] * imag[i]) / (float)half_n;
    }
}
