/**
  * @file pso.h
  * @description A definition of functions offered by the pso
  */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "complex.h"
#include "array.h"
#include <gsl/gsl_rng.h>
#include "utils.h"

#ifndef _PSO_H
#define _PSO_H

/**
 * PSO struct
*/
struct PSO {
    enum AlgorithmType* option;
    size_t* num_dimensions;
    double* global_best_residual_norm;
    double* global_best_iteration_mean;
    double* global_best_iteration_std;
    struct array* global_best_positions;
    double* global_best_w;
    double* global_best_c1;
    double* global_best_c2;
    struct array* position_bounds;
    struct array* init_velocities;
    size_t* iterations_max;
    size_t* num_particles;
    int* seed;
    gsl_rng* rng;
    pthread_mutex_t* mutex;
    char* simulators_directory;
    int* order;
    size_t* k;
    char* dd_name;
    char* dd_directory;
    int* num_workers;
};

/**
 * Create new pso
 * RETURNS:
 *  - PSO struct pointer
 * INPUTE:
 * 
*/
struct PSO* new_pso(enum AlgorithmType* option, size_t* num_dimensions, struct array* position_bounds, size_t* iterations_max, size_t* num_particles, int* seed, char* simulators_directory, int* order, size_t* k, char* dd_name, char* dd_directory, int* num_workers);

/**
 * Free pso
 * RETURNS:
 * - None
 * INPUTE:
 * - PSO struct pointer
*/
void free_pso(struct PSO* pso);

/**
 * Free pso
 * RETURNS:
 * - None
 * INPUTE:
 * - PSO struct pointer
*/
void start_pso(struct PSO* pso);

/**
 * Start pso and do a singel run
 * RETURNS:
 * - 0 if no more to do and 1 if there is still stuff to do 
 * INPUTE:
 * - PSO struct pointer
*/
int start_single_pso(struct PSO* pso);

/**
 * Start pso and do a singel run
 * RETURNS:
 * - 0 if no more to do and 1 if there is still stuff to do 
 * INPUTE:
 * - PSO struct pointer
*/
void start_single_pso_local(struct PSO* pso);

/**
 * Generate random position bound1
 * This is for StEFCal
 * RETURNS:
 * - Random Position array
 * INPUTE:
 * - PSO struct pointer
*/
struct array* random_position_bound1_pso(struct PSO* pso);

/**
 * Generate random position bound2
 * This is for StEFCal Improved
 * RETURNS:
 * - Random Position array
 * INPUTE:
 * - PSO struct pointer
*/
struct array* random_position_bound2_pso(struct PSO* pso);

/**
 * Generate random position
 * RETURNS:
 * - Random Position array
 * INPUTE:
 * - PSO struct pointer
*/
struct array* random_position_pso(struct PSO* pso);

/**
 * Generate random value
 * RETURNS:
 * - Random value
 * INPUTE:
 * - PSO struct pointer
*/
double generate_random_number_pso(struct PSO* pso, size_t* dimension);

/**
 * Resets Global best values
 * RETURNS:
 * - None
 * INPUTE:
 * - PSO struct pointer
*/
void reset_pso(struct PSO* pso);

#endif