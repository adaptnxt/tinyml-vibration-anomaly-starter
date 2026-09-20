"""Synthetic 3-axis industrial vibration signal generator."""

import numpy as np
from typing import Tuple, Optional


class SyntheticVibrationGenerator:
    """Generates synthetic vibration data for machine baseline and mechanical fault profiles."""

    def __init__(self, sample_rate_hz: float = 1000.0, motor_rpm: float = 1800.0):
        self.sample_rate_hz = sample_rate_hz
        self.fundamental_freq_hz = motor_rpm / 60.0  # 1800 RPM = 30 Hz

    def generate_baseline(self, num_samples: int = 256, noise_std: float = 0.05) -> np.ndarray:
        """Generates healthy baseline motor vibration (fundamental sine + harmonics + Gaussian noise)."""
        t = np.arange(num_samples) / self.sample_rate_hz
        f0 = self.fundamental_freq_hz
        signal = (
            1.0 * np.sin(2 * np.pi * f0 * t) +
            0.2 * np.sin(2 * np.pi * (2 * f0) * t) +
            np.random.normal(0, noise_std, num_samples)
        )
        return signal.astype(np.float32)

    def generate_bearing_fault(
        self,
        num_samples: int = 256,
        impact_interval: int = 16,
        impact_amplitude: float = 3.5
    ) -> np.ndarray:
        """Generates outer race bearing defect signal (periodic high-kurtosis impulse shocks)."""
        base = self.generate_baseline(num_samples=num_samples, noise_std=0.08)
        fault_signal = base.copy()
        for i in range(0, num_samples, impact_interval):
            fault_signal[i] += impact_amplitude
            if i + 1 < num_samples:
                fault_signal[i + 1] += impact_amplitude * 0.4  # Ring-down decay
        return fault_signal.astype(np.float32)

    def generate_imbalance(self, num_samples: int = 256, severity: float = 2.5) -> np.ndarray:
        """Generates mechanical unbalance signal (massive amplification at 1X rotational frequency)."""
        t = np.arange(num_samples) / self.sample_rate_hz
        f0 = self.fundamental_freq_hz
        signal = (
            severity * np.sin(2 * np.pi * f0 * t) +
            np.random.normal(0, 0.05, num_samples)
        )
        return signal.astype(np.float32)
