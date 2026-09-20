/**
 * @file test_runner.h
 * @description One entry point per unit-test suite, called by test_runner.c.
 */

#ifndef _TEST_RUNNER_H
#define _TEST_RUNNER_H

int run_array_tests(void);
int run_calibrator_tests(void);
int run_data_container_tests(void);
int run_data_dictionary_tests(void);
int run_data_store_tests(void);
int run_datacollector_tests(void);
int run_jagged_array_tests(void);
int run_particle_tests(void);
int run_progress_bar_tests(void);
int run_pso_tests(void);
int run_simulator_tests(void);
int run_stopwatch_tests(void);
int run_thread_args_tests(void);

#endif
