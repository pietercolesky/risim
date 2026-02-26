/**
 * @file pso_progress.h
 * @description A definition of functions offered by the pso_progress
 */
#include <stdio.h>
#include <pthread.h>
#ifndef PSO_PROGRESS_H
#define PSO_PROGRESS_H

/**
 * PSOProgress struct
 */
struct PSOProgress {
    size_t* barWidth;
    size_t* pso_count_max;
    size_t* iteration_max;
    size_t* num_particles;
    size_t* pso_count;
    size_t* pso_no_effect;
    size_t* iteration;
    size_t* particle;
    size_t* progress;
    size_t* totalSteps;
    float* progressRatio;
    struct Stopwatch* stopwatch;
};

/**
 * This function creates a progress bar struct.
 * RETURNS:
 * - PSOProgress struct pointer
 * INPUTS:
 * - barWidth
 * - pso_count_max
 * - iteration_max
 * - num_particles
 */
struct PSOProgress* new_pp(size_t* barWidth, size_t* pso_count_max, size_t* iteration_max, size_t* num_particles);

/**
 * This frees a PSOProgress struct
 * RETURNS:
 * - NONE
 * INPUTS:
 * - PSOProgress struct pointer
 * - message - stop message
 */
void stop_free_pp(struct PSOProgress* pso_progress, char* message);

/**
 * This function will increment the pso of pso_progress
 * RETURNS:
 * - NONE
 * INPUT:
 * - PSOProgress struct pointer
 */
void increment_pso_pp(struct PSOProgress* pso_progress);

/**
 * This function will increment the iterations of pso_progress
 * RETURNS:
 * - NONE
 * INPUT:
 * - PSOProgress struct pointer
 */
void increment_iterations_pp(struct PSOProgress* pso_progress);

/**
 * This function will increment the pso no effect of pso_progress
 * RETURNS:
 * - NONE
 * INPUT:
 * - PSOProgress struct pointer
 */
void increment_pso_no_effect_pp(struct PSOProgress* pso_progress);

/**
 * This function will increment the particle of pso_progress
 * RETURNS:
 * - NONE
 * INPUT:
 * - PSOProgress struct pointer
 */
void increment_particle_pp(struct PSOProgress* pso_progress);

/**
 * This function will update the progress bar and show ETA
 * RETURNS:
 * - NONE
 * INPUT:
 * - PSOProgress struct pointer
 */
void update_pp(struct PSOProgress* pso_progress);

/**
 * This function calculates current progress based on PSO, iteration, and particle counts
 * RETURNS:
 * - current progress value
 * INPUT:
 * - PSOProgress struct pointer
 */
size_t calculate_progress_pp(struct PSOProgress* pso_progress);

/**
 * This function estimates time remaining
 * RETURNS:
 * - estimated seconds remaining
 * INPUT:
 * - PSOProgress struct pointer
 */
double estimate_time_remaining_pp(struct PSOProgress* pso_progress);

#endif