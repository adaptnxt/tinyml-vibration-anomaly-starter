"""Unit tests for statistical feature extraction and FFT."""

import numpy as np
from tinyml_vibration.features import VibrationFeatureExtractor


def test_rms_and_p2p_calculation():
    extractor = VibrationFeatureExtractor(sample_rate_hz=1000.0)
    # DC signal of constant 2.0
    sig = np.full(256, 2.0, dtype=np.float32)
    features = extractor.extract(sig)
    assert abs(features["rms"] - 2.0) < 1e-4
    assert abs(features["peak_to_peak"] - 0.0) < 1e-4


def test_fft_dominant_frequency_detection():
    extractor = VibrationFeatureExtractor(sample_rate_hz=1000.0)
    t = np.arange(256) / 1000.0
    # Pure 100 Hz sine wave
    sig = np.sin(2 * np.pi * 100.0 * t).astype(np.float32)
    features = extractor.extract(sig)
    # FFT bin resolution for 256 samples at 1000 Hz is ~3.9 Hz, so dominant freq should be close to 100 Hz
    assert abs(features["dominant_freq_hz"] - 100.0) < 5.0
