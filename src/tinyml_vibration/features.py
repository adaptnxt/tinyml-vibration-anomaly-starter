"""Feature extraction matching embedded C firmware mathematics."""

import numpy as np
from typing import Dict, Any


class VibrationFeatureExtractor:
    """Extracts statistical and FFT frequency features from time-domain accelerometer buffers."""

    FEATURE_NAMES = ["rms", "peak_to_peak", "crest_factor", "kurtosis", "dominant_freq_hz"]

    def __init__(self, sample_rate_hz: float = 1000.0):
        self.sample_rate_hz = sample_rate_hz

    def extract(self, signal: np.ndarray) -> Dict[str, float]:
        """Calculates statistical & spectral features.
        
        Args:
            signal: 1D numpy array of acceleration values.
            
        Returns:
            Dictionary with computed features.
        """
        n = len(signal)
        rms = float(np.sqrt(np.mean(signal ** 2)))
        p2p = float(np.max(signal) - np.min(signal))

        # Kurtosis
        mean = np.mean(signal)
        variance = np.var(signal)
        if variance < 1e-7:
            kurtosis = 3.0
        else:
            kurtosis = float(np.mean((signal - mean) ** 4) / (variance ** 2))

        # Crest Factor
        peak_abs = max(abs(float(np.max(signal))), abs(float(np.min(signal))))
        crest_factor = float(peak_abs / rms) if rms > 1e-6 else 0.0

        # Radix-2 FFT spectral analysis
        window = 0.5 * (1.0 - np.cos(2.0 * np.pi * np.arange(n) / (n - 1)))
        windowed = signal * window
        fft_res = np.fft.rfft(windowed)
        magnitudes = np.abs(fft_res) / (n / 2)

        # Ignore DC component (bin 0)
        if len(magnitudes) > 1:
            dominant_bin = int(np.argmax(magnitudes[1:])) + 1
            freqs = np.fft.rfftfreq(n, d=1.0 / self.sample_rate_hz)
            dominant_freq_hz = float(freqs[dominant_bin])
        else:
            dominant_freq_hz = 0.0

        return {
            "rms": rms,
            "peak_to_peak": p2p,
            "crest_factor": crest_factor,
            "kurtosis": kurtosis,
            "dominant_freq_hz": dominant_freq_hz
        }

    def to_vector(self, features: Dict[str, float]) -> np.ndarray:
        """Converts feature dict to ordered array."""
        return np.array([features[name] for name in self.FEATURE_NAMES], dtype=np.float32)
