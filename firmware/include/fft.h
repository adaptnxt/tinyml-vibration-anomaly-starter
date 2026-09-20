/**
 * @file fft.h
 * @brief High-performance embedded in-place Radix-2 FFT for microcontrollers.
 * @author AdaptNXT Technology Solutions
 */

#ifndef ADAPTNXT_TINYML_FFT_H
#define ADAPTNXT_TINYML_FFT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Applies Hanning window in-place to reduce spectral leakage.
 * @param data Array of signal amplitudes
 * @param length Number of samples (must be a power of 2)
 */
void apply_hanning_window(float *data, size_t length);

/**
 * @brief Computes in-place Radix-2 Fast Fourier Transform.
 * @param real Array of real components (input and output)
 * @param imag Array of imaginary components (zeroed before input, output)
 * @param n Size of the buffer (must be a power of 2, e.g. 64, 128, 256)
 */
void compute_radix2_fft(float *real, float *imag, size_t n);

/**
 * @brief Computes magnitude spectrum from real and imaginary FFT components.
 * @param real Real component array of length n
 * @param imag Imaginary component array of length n
 * @param magnitudes Output array of length n/2
 * @param n Number of FFT points
 */
void compute_fft_magnitudes(const float *real, const float *imag, float *magnitudes, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* ADAPTNXT_TINYML_FFT_H */
