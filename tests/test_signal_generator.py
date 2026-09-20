"""Unit tests for synthetic vibration signal generator."""

import numpy as np
from tinyml_vibration.signal_generator import SyntheticVibrationGenerator


def test_baseline_signal_generation():
    gen = SyntheticVibrationGenerator(sample_rate_hz=1000.0, motor_rpm=3600.0)
    sig = gen.generate_baseline(num_samples=256)
    assert len(sig) == 256
    assert isinstance(sig, np.ndarray)
    assert sig.dtype == np.float32
    # Baseline amplitude should oscillate within normal bounds
    assert np.max(sig) < 2.5
    assert np.min(sig) > -2.5


def test_bearing_fault_has_high_peaks():
    gen = SyntheticVibrationGenerator(sample_rate_hz=1000.0, motor_rpm=3600.0)
    baseline = gen.generate_baseline(num_samples=256)
    fault = gen.generate_bearing_fault(num_samples=256, impact_amplitude=4.0)

    assert np.max(fault) > np.max(baseline)
    assert np.max(fault) >= 3.5
