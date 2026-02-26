/**
  * @file pso.h
  * @description A definition of functions offered by the pso
  */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "complex.h"
#include "array.h"
#include <stdbool.h>
#include "pso.h"

#ifndef _Particle_H
#define _Particle_H

/**
 * Particle struct
*/
struct Particle {
    struct PSO* pso;
    size_t* num_dimensions;
    double* w;
    double* c1;
    double* c2;
    struct array* positions;
    struct array* velocities;
    struct array* pb_positions;
    struct array* pb_velocities;
    //Variables that determine the fitness of the particle
    double* current_residual_norm;
    double* current_iterations_mean;
    double* current_iterations_std;
    double* pb_residual_norm;
    double* pb_iterations_mean;
    double* pb_iterations_std;
    size_t* k;

};

/**
 * Create new Particle
 * RETURNS:
 *  - Particle struct pointer
 * INPUTE:
 *  - num_dimensions
 *  - w
 *  - c1
 *  - c2
 *  - positions
 *  - velocities
*/
struct Particle* new_particle(struct PSO* pso, size_t* num_dimensions, struct array* positions, struct array* velocities);

/**
 * Free Particle
 * RETURNS:
 *  - None
 * INPUTE:
 *  - struct Particle pointer
*/
void free_particle(struct Particle* particle);

/**
 * Update position
 * RETURNS:
 *  - None
 * INPUTE:
 *  - dimension
 *  - position
*/
void update_position_particle(struct Particle* particle);

/**
 * Update position Bound
 * RETURNS:
 *  - None
 * INPUTE:
 *  - dimension
 *  - position
*/
void update_position_bounds_particle(struct Particle* particle);

/**
 * Update velocity
 * RETURNS:
 *  - None
 * INPUTE:
 *  - dimension
 *  - velocity
*/
void update_velocity_particle(struct Particle* particle);

/**
 * Update hyperparameters
 * RETURNS:
 *  - None
 * INPUTE:
 *  - struct Particle pointer
*/
void update_hyperparameters_particle(struct Particle* particle);

/**
 * Check if hyperparameters needs to be updated
 * RETURNS:
 *  - None
 * INPUTE:
 *  - struct Particle pointer
*/
void check_update_hyperparameters_particle(struct Particle* particle, size_t* k);

/**
 * Update fitness
 * This function will also update the pb_position and pb velocity
 * RETURNS:
 *  - None
 * INPUTE:
 *  - pb_residual_mean
 *  - pb_iterations_mean
 *  - pb_iterations_std
*/
void update_fitness_particle(struct Particle* particle);

/**
 * Check if fitness needs to be updated
 * RETURNS:
 *  - None
 * INPUTE:
 *  - struct Particle pointer
*/
void check_update_fitness_particle(struct Particle* particle);

#endif