# Contributing to AdaptNXT TinyML Vibration Anomaly Starter

Thank you for contributing to the **TinyML Vibration Anomaly Starter** maintained by [AdaptNXT Technology Solutions](https://www.adaptnxt.com).

## Development Setup

1. **Fork and clone** this repository.
2. **Python Environment**:
   ```bash
   python -m venv .venv
   source .venv/bin/activate # On Windows: .venv\Scripts\activate
   pip install -e ".[dev]"
   ```
3. **Run Python Tests**:
   ```bash
   pytest tests/ -v
   ```
4. **Compile C Firmware locally**:
   ```bash
   gcc -I firmware/include firmware/src/fft.c firmware/src/vibration_analyzer.c firmware/main.c -lm -o vibration_sentinel
   ./vibration_sentinel
   ```

## Embedded Guidelines
- Embedded C code must be C99 standard compliant.
- No dynamic memory allocation (`malloc`/`free`) in the real-time FFT/inference loop to prevent heap fragmentation in long-running industrial deployments.
- Code should compile warning-free under `-Wall -Wextra`.
