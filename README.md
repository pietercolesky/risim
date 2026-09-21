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

Drop `-g` and `-w` to reproduce the paper's actual grid — see [Runtimes](#runtimes) first.

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
5. **extraction** — turn the stored results into the `.txt`, `.array` and `.csv` files
   the Python plotting scripts read: `heatmap*`, `pso-*`, `perf-*`, `points-*`,
   `pbest-*` and `iterations-*`.

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

# convergence tables, optimized against default
./risim perf-$S-opt -o 2 10
./risim perf-$S-def -o 2 10

# scatter points and confidence ellipses
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
workstation. Use `-g`, `-i` and `-p` to explore the pipeline locally, and expect the
absolute numbers from a reduced run to differ from the paper's.

---

## Tests

```bash
./run.sh -u          # or: make test && ./unit_test_runner
```

`unit_test/test_runner.c` runs all thirteen suites (440 tests) and exits with the number
of failures. Two groups are excluded from the default run and can be re-enabled in the
relevant `run_*_tests()` function:

- tests that call gnuplot, which only pass where gnuplot is installed;
- integration tests in `test_data_store.c` and `test_pso.c`, which need datasets
  generated beforehand and run a full search.

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

Generated data is written under the data directory as `simulators/` (datasets),
`data/` (search results), `extracted_heatmap_data/` and `data_optimal_solutions/`
(extracted results). None of it is tracked by git.

---

## Licence and citation

Released under the [MIT Licence](LICENSE). If you use this software, please cite the
paper — see [CITATION.cff](CITATION.cff).
