/**
 * @file test_runner.c
 * @description Runs every unit-test suite and reports the total number of
 * failures. Unity requires a single setUp/tearDown pair, so they live here.
 */

#include <stdio.h>
#include "unity.h"
#include "test_runner.h"

void setUp(void) {}
void tearDown(void) {}

int main(void) {
    int failures = 0;

    printf("\n--- array ---\n");
    failures += run_array_tests();
    printf("\n--- calibrator ---\n");
    failures += run_calibrator_tests();
    printf("\n--- data_container ---\n");
    failures += run_data_container_tests();
    printf("\n--- data_dictionary ---\n");
    failures += run_data_dictionary_tests();
    printf("\n--- data_store ---\n");
    failures += run_data_store_tests();
    printf("\n--- datacollector ---\n");
    failures += run_datacollector_tests();
    printf("\n--- jagged_array ---\n");
    failures += run_jagged_array_tests();
    printf("\n--- particle ---\n");
    failures += run_particle_tests();
    printf("\n--- progress_bar ---\n");
    failures += run_progress_bar_tests();
    printf("\n--- pso ---\n");
    failures += run_pso_tests();
    printf("\n--- simulator ---\n");
    failures += run_simulator_tests();
    printf("\n--- stopwatch ---\n");
    failures += run_stopwatch_tests();
    printf("\n--- thread_args ---\n");
    failures += run_thread_args_tests();

    printf("\n=== %d failure(s) in total ===\n", failures);
    return failures;
}
