# RISim — Radio Interferometric Simulation and Calibration

A radio interferometric simulator with an integrated calibration pipeline, written in C.
RISim simulates interferometric observations and provides a framework for running,
tuning and comparing antenna-based gain calibration solvers.

This repository contains the simulator, the solvers and the experiment pipeline used in
*Hyperparameter Optimization of Calibration Solvers* (Colesky, Grobler & Kleynhans).

Questions or requests for algorithms not included here: Pieter Colesky,
[pietercolesky@gmail.com](mailto:pietercolesky@gmail.com).

## Solvers

| Solver | Entry point | Hyperparameters |
| --- | --- | --- |
| StEFCal | `solve_G_StEFCal` | — |
| StEFCal with alternating damping | `solve_G_StEFCal_Lambda` | `lambda_even`, `lambda_odd` |
| NOS (non-diagonal StEFCal) | `solve_G_StEFCal_improved`, `solve_G_NOS_fixed` | `lambda_even`, `lambda_odd` |
| SAS (stochastic adaptive StEFCal) | `solve_G_StEFCal_stochastic_adaptive_l1l2` | `lambda_even`, `lambda_odd`, `b0` |
| Levenberg–Marquardt, adaptive | `solve_G_LM` | `rho`, `nu` |
| Levenberg–Marquardt, non-adaptive | `solve_G_LM_Non_Adaptive_l1_l2` | `lambda_even`, `lambda_odd` |
| ADAM | `solve_G_ADAM`, `solve_G_ADAM_without_r` | `alpha`, `beta_1`, `beta_2` |

All solvers are declared in `include/calibrator.h` and parallelise over antennae.

## Building

Dependencies: `gcc`, GSL, LAPACK/LAPACKE, BLAS, `libuuid`, pthreads.
`gnuplot` and `gnuplot_i` are needed only for the plotting helpers.

```bash
make local      # build ./risim
make test       # build ./unit_test_runner
make hpc        # build with the HPC library paths
```

`run.sh` wraps the scripts in `scripts/`:

| Option | Description |
| --- | --- |
| `-h`, `--help` | Show the help message |
| `-l`, `--local` | Build and run locally |
| `-u`, `--unit-test` | Run the unit tests |
| `-uv`, `--unit-valgrind` | Run the unit tests under Valgrind |
| `-v`, `--valgrind` | Run under Valgrind |
| `--hpc` | Run the HPC build |

Arguments after the option are passed through, so `./run.sh -l simulate -o 2 2` works.

## Running

```bash
./risim                       # list every command
./risim <command> [options]
```

Options:

| Option | Meaning |
| --- | --- |
| `-d <directory>` | data directory (default `./simulation_environment`) |
| `-o <min> <max>` | antenna order range, inclusive (default `2 2`) |
| `-a <algorithm>` | solver the command applies to (default `stefcal`) |
| `-s <index>` | PSO seed index, 0–3 (default `0`) |

An antenna order *n* describes an array of *n*×*n* antennae, so order 2 is 4 antennae.
Algorithm names are `stefcal`, `nos`, `lm-rv`, `lm-eo`, `lm-rv-rt`, `lm-eo-rt`, `adam`,
`adam-fixed`, `sas`, `sas-l1l2` and `sas-adaptive`, where `lm-rv` is LM (rho, nu) and
`lm-eo` is LM (lambda_even, lambda_odd).

### Getting started

```bash
make local
./risim simulate -o 2 2      # 64 datasets: 4 seeds x 4 SNRs x 4 gain amplitudes
```

### Reproducing the paper's results

Each stage reads what the previous one wrote under the data directory.

```bash
# 1. datasets
./risim simulate -o 2 10

# 2. data dictionaries for the solver you are tuning
./risim dict-grid -a stefcal -o 2 10
./risim dict-pso  -a stefcal -o 2 10

# 3. grid search over the hyperparameter plane (the heatmaps)
./risim grid -a stefcal -o 2 10

# 4. particle-swarm search, once per seed
for s in 0 1 2 3; do ./risim pso -a stefcal -s $s -o 2 10; done

# 5. extract what the plotting scripts read
./risim heatmap-orders -a stefcal -o 2 10
./risim pso-best       -a stefcal -o 2 10
./risim perf-stefcal-opt -o 2 10
./risim perf-stefcal-def -o 2 10
./risim points-stefcal-opt -o 2 10
```

ADAM and SAS are tuned in two rounds: the first round searches the full box, then
`best-alpha-adam` / `best-b-sas` fix one hyperparameter and `grid-adam` / `grid-sas` and
`pso-adam` / `pso-sas` search the remaining plane.

The grid search uses a 500×500 grid over 64 datasets per antenna order, so the full
sweep is an HPC-scale run rather than something to try on a laptop.

## Tests

```bash
./run.sh -u
```

`unit_test/test_runner.c` runs all thirteen suites (440 tests) and returns the number of
failures. Two groups are left out of the default run and can be re-enabled in the
relevant `run_*_tests()` function:

- tests that call gnuplot, which only pass where gnuplot is installed;
- integration tests in `test_data_store.c` and `test_pso.c`, which need datasets
  generated beforehand and run a full search.

## Layout

| Path | Contents |
| --- | --- |
| `src/`, `include/` | simulator, calibrator, solvers, PSO, data store and dictionary |
| `src/main.c` | the command line and the experiment pipeline |
| `unit_test/` | Unity test suites and the runner |
| `lib/` | vendored [Unity](https://github.com/ThrowTheSwitch/Unity) |
| `scripts/` | build-and-run wrappers used by `run.sh` |
| `simulation_environment/` | default data directory (generated content is ignored) |
