"""Command-line interface for AdaptNXT TinyML Vibration Anomaly Detection."""

import argparse
import sys
import numpy as np
from tinyml_vibration.signal_generator import SyntheticVibrationGenerator
from tinyml_vibration.features import VibrationFeatureExtractor
from tinyml_vibration.model import VibrationAnomalyDetector


def main():
    parser = argparse.ArgumentParser(
        description="AdaptNXT TinyML Vibration Anomaly Pipeline & Firmware Exporter"
    )
    subparsers = parser.add_subparsers(dest="command", help="Available subcommands")

    # simulate
    sim_parser = subparsers.add_parser("simulate", help="Simulate vibration signal and run anomaly detection")
    sim_parser.add_argument("--fault", choices=["none", "bearing", "imbalance"], default="none", help="Fault type")
    sim_parser.add_argument("--samples", type=int, default=256, help="Number of buffer samples")

    # export-header
    export_parser = subparsers.add_parser("export-header", help="Generate embedded C model header file")
    export_parser.add_argument("--output", type=str, default="firmware/include/model_data.h", help="Output .h path")

    args = parser.parse_args()

    generator = SyntheticVibrationGenerator(sample_rate_hz=1000.0, motor_rpm=3600.0) # 60 Hz fundamental
    extractor = VibrationFeatureExtractor(sample_rate_hz=1000.0)
    detector = VibrationAnomalyDetector(threshold=2.5)

    if args.command == "simulate":
        if args.fault == "bearing":
            signal = generator.generate_bearing_fault(num_samples=args.samples)
            condition = "Bearing Outer Race Fault"
        elif args.fault == "imbalance":
            signal = generator.generate_imbalance(num_samples=args.samples)
            condition = "Rotational Imbalance"
        else:
            signal = generator.generate_baseline(num_samples=args.samples)
            condition = "Healthy Baseline"

        features = extractor.extract(signal)
        vec = extractor.to_vector(features)
        is_anomalous, score = detector.predict(vec)

        print(f"\n=======================================================")
        print(f" AdaptNXT TinyML Vibration Sentinel Simulation")
        print(f" Condition:       {condition}")
        print(f" Buffer Size:     {args.samples} samples (1000 Hz)")
        print(f"-------------------------------------------------------")
        print(f" RMS:             {features['rms']:.3f} G")
        print(f" Peak-to-Peak:    {features['peak_to_peak']:.3f} G")
        print(f" Crest Factor:    {features['crest_factor']:.2f}")
        print(f" Kurtosis:        {features['kurtosis']:.2f}")
        print(f" Dominant Freq:   {features['dominant_freq_hz']:.1f} Hz")
        print(f"-------------------------------------------------------")
        print(f" Anomaly Score:   {score:.2f} (Threshold: {detector.threshold:.2f})")
        print(f" Status:          {'🚨 ANOMALY DETECTED' if is_anomalous else '✅ NORMAL (PASSED)'}")
        print(f"=======================================================\n")

    elif args.command == "export-header":
        # Generate calibration baseline
        baseline_vectors = []
        for _ in range(50):
            sig = generator.generate_baseline(num_samples=256)
            feat = extractor.extract(sig)
            baseline_vectors.append(extractor.to_vector(feat))
        detector.fit(baseline_vectors, threshold_percentile=98.0)
        detector.export_c_header(args.output)
        print(f"Successfully generated embedded C header at: {args.output}")

    else:
        parser.print_help()


if __name__ == "__main__":
    main()
