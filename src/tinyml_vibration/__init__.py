"""AdaptNXT TinyML Vibration Anomaly Starter."""

from tinyml_vibration.signal_generator import SyntheticVibrationGenerator
from tinyml_vibration.features import VibrationFeatureExtractor
from tinyml_vibration.model import VibrationAnomalyDetector

__version__ = "0.1.0"
__author__ = "AdaptNXT Technology Solutions"

__all__ = [
    "SyntheticVibrationGenerator",
    "VibrationFeatureExtractor",
    "VibrationAnomalyDetector",
]
