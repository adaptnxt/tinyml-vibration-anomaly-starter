"""Unit tests for anomaly scoring and C-header export."""

import os
import tempfile
import numpy as np
from tinyml_vibration.signal_generator import SyntheticVibrationGenerator
from tinyml_vibration.features import VibrationFeatureExtractor
from tinyml_vibration.model import VibrationAnomalyDetector


def test_model_detects_bearing_fault_anomaly():
    gen = SyntheticVibrationGenerator(sample_rate_hz=1000.0)
    extractor = VibrationFeatureExtractor(sample_rate_hz=1000.0)
    detector = VibrationAnomalyDetector(threshold=2.5)

    # Train on 20 normal samples
    train_vecs = [extractor.to_vector(extractor.extract(gen.generate_baseline())) for _ in range(20)]
    detector.fit(train_vecs, threshold_percentile=95.0)

    # Test healthy signal
    healthy_sig = gen.generate_baseline()
    healthy_feat = extractor.to_vector(extractor.extract(healthy_sig))
    is_anom, score_healthy = detector.predict(healthy_feat)
    assert is_anom is False

    # Test bearing fault
    fault_sig = gen.generate_bearing_fault(impact_amplitude=5.0)
    fault_feat = extractor.to_vector(extractor.extract(fault_sig))
    is_fault, score_fault = detector.predict(fault_feat)
    assert is_fault is True
    assert score_fault > score_healthy


def test_export_c_header():
    detector = VibrationAnomalyDetector(threshold=3.0)
    with tempfile.NamedTemporaryFile(suffix=".h", delete=False) as f:
        temp_path = f.name

    try:
        detector.export_c_header(temp_path, version="1.0.0")
        assert os.path.exists(temp_path)
        with open(temp_path, "r") as f:
            content = f.read()
        assert "ADAPTNXT_MODEL_DATA_H" in content
        assert "VIBRATION_BASELINE_MEANS" in content
        assert "VIBRATION_THRESHOLD" in content
    finally:
        if os.path.exists(temp_path):
            os.remove(temp_path)
