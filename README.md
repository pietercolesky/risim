# RISim — Radio Interferometric Simulation and Calibration

A radio interferometric simulator with an integrated calibration pipeline, written in C.
RISim simulates interferometric observations and provides a framework for running, tuning
and comparing antenna-based gain calibration solvers.

This repository contains the simulator, the solvers and the full experiment pipeline used
in *Hyperparameter Optimization of Calibration Solvers* (Colesky, Grobler & Kleynhans,
MNRAS). Every number in the paper can be regenerated with the commands below.

Contact: Pieter Colesky, [pietercolesky@gmail.com](mailto:pietercolesky@gmail.com).

---

## Contents

- [Solvers](#solvers)
- [Building](#building)
- [Quick start](#quick-start)
- [How the pipeline works](#how-the-pipeline-works)
- [Command reference](#command-reference)
- [Reproducing each experiment](#reproducing-each-experiment)
- [Runtimes](#runtimes)
- [Tests](#tests)
- [Repository layout](#repository-layout)
- [Licence and citation](#licence-and-citation)

---

## Solvers

| Paper name | CLI name | Entry point | Tuned hyperparameters |
| --- | --- | --- | --- |
| StEFCal | `stefcal` | `solve_G_StEFCal_Lambda` | `lambda_even`, `lambda_odd` |
| NOS (non-diagonal StEFCal) | `nos` | `solve_G_StEFCal_improved` | `lambda_even`, `lambda_odd` |
| LM (rho, nu) | `lm-rv` | `solve_G_LM` | `rho`, `nu` |
| LM (lambda_even, lambda_odd) | `lm-eo` | `solve_G_LM_Non_Adaptive_l1_l2` | `lambda_even`, `lambda_odd` |
| ADAM | `adam`, `adam-fixed` | `solve_G_ADAM`, `solve_G_ADAM_without_r` | `alpha`, `beta_1`, `beta_2` |
| SAS (stochastic adaptive StEFCal) | `sas`, `sas-l1l2`, `sas-adaptive` | `solve_G_StEFCal_stochastic_adaptive_l1l2` | `lambda_even`, `lambda_odd`, `b0` |

`lm-rv-rt` and `lm-eo-rt` are the reduced-timestep forms of the two LM solvers.
Plain `solve_G_StEFCal` (undamped) and `solve_G_NOS_fixed` are also available.
All solvers are declared in `include/calibrator.h` and parallelise over antennae.

---

## Building

Dependencies: a C compiler, GSL, LAPACK/LAPACKE, BLAS, `libuuid` and pthreads.
`gnuplot` plus `gnuplot_i` are needed only by the optional plotting helpers.

On Debian or Ubuntu:

```bash
sudo apt install build-essential libgsl-dev liblapacke-dev libblas-dev uuid-dev
```

Then:

```bash
make local      # build ./risim
make test       # build ./unit_test_runner
make hpc        # build against the HPC LAPACK paths
make clean      # remove objects and binaries
```

`run.sh` wraps the scripts in `scripts/`:

| Option | Description |
| --- | --- |
| `-l`, `--local` | Build and run locally |
| `-u`, `--unit-test` | Run the unit tests |
| `-uv`, `--unit-valgrind` | Run the unit tests under Valgrind |
| `-v`, `--valgrind` | Run under Valgrind |
| `--hpc` | Run the HPC build |

Arguments after the option are passed through: `./run.sh -l simulate -o 2 2`.

---

## Quick start

This takes a few minutes and exercises the whole chain at a size that finishes on a
workstation. It is the fastest way to confirm the build is sound.

```bash
make local

# 1. Generate the datasets for a 4-antenna array (order 2).
#    64 datasets: 4 seeds x 4 SNRs x 4 gain amplitudes.
./risim simulate -o 2 2

# 2. Create the data dictionary that indexes the grid search.
./risim dict-grid -a stefcal -o 2 2

# 3. Grid search over the (lambda_even, lambda_odd) plane.
#    -g 2 uses a 2x2 grid instead of the published 500x500. ~2 minutes.
./risim grid -a stefcal -o 2 2 -g 2 -w 8

# 4. Extract what the plotting scripts read.
./risim heatmap-orders -a stefcal -o 2 2
```

Step 4 writes `iteration_means.array`, `iteration_stds.array` and `residual_norms.array`
per dataset under `simulation_environment/extracted_heatmap_data/`.

To exercise the swarm branch and the extraction that depends on it as well:

```bash
./risim dict-pso -a stefcal -o 2 2
for s in 0 1 2 3; do ./risim pso -a stefcal -s $s -o 2 2 -i 2 -p 2 -w 8; done
./risim pso-best  -a stefcal -o 2 2     # ellipses and scatter points
./risim perf-stefcal-opt -o 2 2 -w 8    # re-run at the hyperparameters found
./risim perf-stefcal-def -o 2 2 -w 8
./risim points-stefcal-opt -o 2 2       # convergence summaries
./risim points-stefcal-def -o 2 2
```

The whole sequence takes about fifteen minutes on a workstation. Drop `-g`, `-i`, `-p`
and `-w` to reproduce the paper — see [Runtimes](#runtimes) first.

---

## How the pipeline works

Every stage reads what the previous one wrote under the data directory (`-d`, default
`./simulation_environment`). The stages are:

1. **simulate** — generate visibility datasets. For each antenna order the simulator
   sweeps 4 random seeds, 4 SNRs (1000, 10, 0, -1 dB) and 4 maximum gain amplitudes
   (2, 3, 5, 10), giving 64 datasets per order.
2. **dictionaries** — create the `.ddconfig` index a search writes its results into.
   `dict-grid` for a grid search, `dict-pso` for a swarm search.
3. **search** — either an exhaustive grid over the hyperparameter plane (`grid*`, which
   produces the heatmaps) or a particle swarm (`pso*`, which produces the optimal
   hyperparameters, the particle traces and the confidence ellipses).
4. **round two** — ADAM and SAS have three hyperparameters. Their first round searches
   the full box; `best-alpha-adam` and `best-b-sas` then fix one hyperparameter so the
   second round searches the remaining plane.
5. **extraction** — turn the stored results into the files the plotting scripts read.
   `heatmap*` and `pso-*` summarise the two searches. `perf-*` then re-runs the solver
   at the optimal (or default) hyperparameters and stores the per-dataset result, and
   `points-*` reduces those into the scatter points, the confidence ellipses and the
   `iterations_mean/std/mad.txt` and `mean_last_residual_norm.txt` summaries. So
   `perf-*` must be run before the matching `points-*`.

An antenna order *n* means an *n*×*n* array, so order 2 is 4 antennae and order 10 is 100.
The paper covers orders 2 to 10.

---

## Command reference

```bash
./risim                       # print every command
./risim <command> [options]
```

| Option | Meaning | Default |
| --- | --- | --- |
| `-d <directory>` | data directory | `./simulation_environment` |
| `-o <min> <max>` | antenna order range, inclusive | `2 2` |
| `-a <algorithm>` | solver the command applies to | `stefcal` |
| `-s <index>` | PSO seed index, 0–3 | `0` |
| `-w <workers>` | calibration threads | as published |
| `-g <points>` | grid points per hyperparameter axis | as published (500) |
| `-i <iterations>` | PSO iterations | as published (50) |
| `-p <particles>` | PSO particles | as published (25) |

`-w`, `-g`, `-i` and `-p` exist so the pipeline can be run at a size that finishes
locally. Leaving them out reproduces the paper exactly; the published worker counts
assume an HPC node, so set `-w` to your core count when running on a workstation.

| Group | Commands |
| --- | --- |
| simulation | `simulate` |
| dictionaries | `dict-grid`, `dict-pso`, `dict-extracted` |
| grid search | `grid`, `grid-adam`, `grid-sas`, `grid-sas-adaptive` |
| pso | `pso`, `pso-lm-rv`, `pso-adam`, `pso-sas`, `pso-sas-l1l2`, `pso-sas-adaptive` |
| round two | `best-alpha-adam`, `best-alpha-adam-all`, `best-alpha-adam-3d`, `best-b-sas`, `best-l1l2-b-sas` |
| heatmaps | `heatmap`, `heatmap-orders` |
| pso output | `pso-traces`, `pso-best`, `pso-best-jarray` |
| performance | `perf-<solver>-opt`, `perf-<solver>-def` |
| points and ellipses | `points-<solver>-opt`, `points-<solver>-def` |
| particle best | `pbest-<solver>`, `pbest-pso-<solver>` |
| iterations | `iterations-opt`, `iterations-def` |
| SAS study | `sas-vs-stefcal`, `sas-vs-dostefcal`, `sas-raw-iterations`, `compare-stefcal-v2` |

`-opt` is the hyperparameter-optimized variant (the paper's DO/TO prefix), `-def` the
solver at its default hyperparameters.

---

## Reproducing each experiment

Each block is self-contained. Replace `-o 2 10` with the orders you want; the paper uses
2 through 10. Add `-w <cores>` throughout when running locally.

### StEFCal, NOS, LM (rho, nu) and LM (lambda_even, lambda_odd)

These four are tuned in a single round, so they share one recipe. Substitute `$ALG` with
`stefcal`, `nos`, `lm-rv` or `lm-eo`, and `$S` with the matching `stefcal`, `nos`,
`lm-rv` or `lm-eo` in the extraction command names.

```bash
./risim simulate -o 2 10

# grid search -> the heatmaps
./risim dict-grid -a $ALG -o 2 10
./risim grid      -a $ALG -o 2 10
./risim heatmap-orders -a $ALG -o 2 10

# swarm search -> optimal hyperparameters, traces, ellipses
./risim dict-pso -a $ALG -o 2 10
for s in 0 1 2 3; do ./risim pso -a $ALG -s $s -o 2 10; done
./risim pso-best   -a $ALG -o 2 10
./risim pso-traces -a $ALG -o 2 10

# re-run the solver at the optimal and at the default hyperparameters
./risim perf-$S-opt -o 2 10
./risim perf-$S-def -o 2 10

# reduce those runs to the scatter points, ellipses and convergence summaries
./risim points-$S-opt -o 2 10
./risim points-$S-def -o 2 10
```

`pso-lm-rv` is a dedicated driver for LM (rho, nu) with that solver's own search box; use
it in place of `pso -a lm-rv` to reproduce the paper.

### ADAM

Three hyperparameters, so two rounds.

```bash
# round 1: swarm over the full (alpha, beta_1, beta_2) box
./risim dict-pso -a adam -o 2 10
for s in 0 1 2 3; do ./risim pso -a adam -s $s -o 2 10; done

# fix alpha at the best value found
./risim best-alpha-adam     -o 2 10
./risim best-alpha-adam-all -o 2 10

# round 2: search the remaining (beta_1, beta_2) plane at that alpha
./risim dict-grid -a adam-fixed -o 2 10
./risim grid-adam -o 2 10
./risim dict-pso  -a adam-fixed -o 2 10
for s in 0 1 2 3; do ./risim pso-adam -s $s -o 2 10; done

./risim perf-adam-opt   -o 2 10
./risim perf-adam-def   -o 2 10
./risim perf-adam-3d    -o 2 10
./risim points-adam-opt -o 2 10
./risim points-adam-def -o 2 10
./risim pbest-adam      -o 2 10
./risim pbest-pso-adam  -o 2 10
```

### SAS

Also three hyperparameters (`lambda_even`, `lambda_odd`, `b0`), so also two rounds.

```bash
# round 1: swarm over the full box
./risim dict-pso      -a sas-l1l2 -o 2 10
for s in 0 1 2 3; do ./risim pso-sas-l1l2 -s $s -o 2 10; done

# fix the initial batch size b0
./risim best-b-sas       -o 2 10
./risim best-l1l2-b-sas  -o 2 10

# round 2: search the remaining lambda plane at that b0
./risim dict-grid -a sas -o 2 10
./risim grid-sas  -o 2 10
./risim dict-pso  -a sas -o 2 10
for s in 0 1 2 3; do ./risim pso-sas -s $s -o 2 10; done

./risim perf-sas-opt   -o 2 10
./risim perf-sas-def   -o 2 10
./risim points-sas-opt -o 2 10
./risim points-sas-def -o 2 10
./risim pbest-sas      -o 2 10
```

The single-lambda variant of SAS uses `grid-sas-adaptive`, `pso-sas-adaptive`,
`perf-sas-adaptive-opt`/`-def` and `points-sas-adaptive-opt`/`-def`.

### SAS against StEFCal — iterations, FLOPs and work-equivalent iterations

This is the comparison behind the paper's claim that SAS needs fewer FLOPs than StEFCal.
It reads the optimal hyperparameters found above, so run the StEFCal and SAS blocks first.

```bash
./risim sas-vs-stefcal     -o 2 10   # SAS against StEFCal
./risim sas-vs-dostefcal   -o 2 10   # SAS against the tuned DOStEFCal
./risim sas-raw-iterations -o 2 10   # raw per-run iteration counts
./risim compare-stefcal-v2           # flat-buffer StEFCal against the original
```

`compare-stefcal-v2` checks that the flat-buffer StEFCal used for timing produces the
same numbers as the original implementation, so the FLOP comparison measures the
algorithm rather than the memory-management style.

---

## Runtimes

The cost of a grid search scales with the number of grid points squared, and every search
runs over all 64 datasets of an antenna order. Measured on a 32-core workstation at order 2:

| Grid | Datasets | Wall clock |
| --- | --- | --- |
| `-g 2` (2×2) | 64 | ~2 minutes |
| `-g 8` (8×8) | 64 | ~35 minutes |
| published (500×500) | 64 | HPC scale |

Cost also grows quickly with antenna order, since the number of baselines grows as *N*².
The published sweep over orders 2 to 10 for all six solvers was run on a cluster, not a
workstation.

Use `-g`, `-i` and `-p` to see the pipeline work, not to obtain results. A reduced run
does not merely lose precision: the search is too coarse to find an optimum at all. A
full pass at `-g 2 -i 2 -p 2` gives StEFCal a mean of 431.3 iterations at the
"optimal" hyperparameters against 425.1 at the default ones — that is, the tuned solver
comes out worse, which is the opposite of the paper's result. Reproducing the paper
means running the published settings.

---

## Tests

```bash
./run.sh -u          # or: make test && ./unit_test_runner
```

`unit_test/test_runner.c` runs all thirteen suites (441 tests) and exits with the number
of failures. Two groups are excluded from the default run and can be re-enabled in the
relevant `run_*_tests()` function:

- tests that call gnuplot, which only pass where gnuplot is installed;
- integration tests in `test_data_store.c` and `test_pso.c`, which need datasets
  generated beforehand and run a full search.

---

## Known issues and changelog

**Noise model (fixed in `v1.1`).** Up to and including tag `v1.0-thesis`, `generate_noise`
in `src/simulator.c` drew the real and imaginary parts of the noise from a uniform
distribution on [0, 1) instead of a standard normal distribution. With the scaling
`sig = sqrt(P/2)` this gave noise with:

- a non-zero mean of `(sig/2)(1 + i)` on every visibility, and
- a power of `P/3` instead of the requested `P`, i.e. a realized SNR about 4.8 dB higher
  than the nominal SNR (the 1000 dB datasets are unaffected).

From `v1.1` the noise is zero-mean complex Gaussian with power `P`, as intended, and
`test_generate_noise_statistics` in `unit_test/test_simulator.c` checks its mean, power and
Hermitian symmetry. The published thesis and paper results were produced with the earlier
noise model; check out `v1.0-thesis` to reproduce them exactly. A comparison of StEFCal and
SAS under both noise models showed the same solver rankings, with the earlier model giving
milder noise than the nominal SNR labels.

---

## Repository layout

| Path | Contents |
| --- | --- |
| `src/`, `include/` | simulator, calibrator, solvers, PSO, data store and data dictionary |
| `src/main.c` | the command line and the experiment pipeline |
| `src/calibrator.c` | all solvers and their per-antenna worker threads |
| `unit_test/` | Unity test suites and `test_runner.c` |
| `lib/` | vendored [Unity](https://github.com/ThrowTheSwitch/Unity) |
| `scripts/` | build-and-run wrappers used by `run.sh` |
| `simulation_environment/` | default data directory; generated content is git-ignored |

Generated data is written under the data directory:

| Directory | Written by | Contents |
| --- | --- | --- |
| `simulators/` | `simulate` | the visibility datasets |
| `data/` | `dict-*`, `grid*`, `pso*` | data dictionaries and the stored search results |
| `extracted_heatmap_data/` | `heatmap*` | `iteration_means`, `iteration_stds`, `residual_norms` per dataset |
| `extracted_trace_data/` | `pso-best`, `pso-traces` | `all_points.txt`, `ellipses_points.txt`, the violin data |
| `data_optimal_solutions/` | `perf-*` | per-dataset results at the optimal and default hyperparameters |
| `data_optimal_solutions_extracted_data_points/` | `points-*` | `iterations_mean/std/mad.txt`, `mean_last_residual_norm.txt` |
| `extracted_p_best/` | `pbest-*` | per-particle best arrays |

None of it is tracked by git.

---

## Licence and citation

Released under the [MIT Licence](LICENSE). If you use this software, please cite the
paper — see [CITATION.cff](CITATION.cff).
