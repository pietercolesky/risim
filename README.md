# Radio Interferometric Simulations (RISim) — A C library for radio interferometric simulation and calibration

A radio interferometric simulator with an integrated calibration pipeline built in C. RISim provides tools for simulating radio interferometry observations and testing various calibration algorithms.

**Note:** Not all calibration algorithms are currently included in this project. If you require additional algorithms or have specific needs, please contact Pieter Colesky at [pietercolesky@gmail.com](mailto:pietercolesky@gmail.com).

## Overview

RISim simulates radio interferometric observations and provides a framework for testing calibration algorithms. The typical workflow is:

1. **Setup Environment**: Ensure the `simulation_environment/` folder exists
2. **Create Simulators**: Use the `create_simulators()` function in `main` to initialize the simulation
3. **Run Calibration**: Execute calibration tests with your chosen algorithm using the `run_calibration_test()` function

## Quick Start

RISim includes a convenient runner script to help you build, run, and test the simulator.

### Using the Runner Script

The `run.sh` script in the root directory provides easy access to all build and test configurations:

```bash
./run.sh [OPTION] [ARGS...]
```

**Available Options:**

| Option | Description |
|--------|-------------|
| `-h`, `--help` | Show help message |
| `-l`, `--local` | Build and run locally |
| `-u`, `--unit-test` | Run unit tests |
| `-uv`, `--unit-valgrind` | Run unit tests with Valgrind memory analysis |
| `-v`, `--valgrind` | Run with Valgrind |
| `--hpc` | Run HPC version |

**Examples:**

```bash
# Display help
./run.sh

# Build and run locally
./run.sh -l

# Run locally with arguments
./run.sh --local arg1 arg2

# Run unit tests
./run.sh -u

# Run with Valgrind memory analysis
./run.sh --valgrind
```

**Note:** The runner script is a wrapper around the scripts in the `scripts/` folder. Any additional arguments after the option are passed to the underlying script.

