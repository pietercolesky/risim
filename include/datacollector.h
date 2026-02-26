/**
  * @file datacollector.h
  * @description A definition of functions offered by the datacollector
  */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "array.h"
#include "jagged_array.h"

#ifndef _DATACOLLECTOR_H
#define _DATACOLLECTOR_H

/**
 * DataCollector struct
*/
struct DataCollector {
    struct Calibrator* calibrator;
    struct array* iterations;
    struct Jaggedarray* gains;
    struct Jaggedarray* all_gains;
    struct Jaggedarray* damping_g;
    struct Jaggedarray* damping_g_old;
    struct Jaggedarray* last_residual;
    pthread_mutex_t* mutex;
};

/**
 * Create new datacollector
 * RETURNS:
 *  - DataCollector struct pointer
 * INPUTE:
 *  - NONE
*/
struct DataCollector* new_dc(struct Calibrator* calibrator);

/**
 * Create new datacollector for reduced data collection
 * RETURNS:
 *  - DataCollector struct pointer
 * INPUTE:
 *  - NONE
*/
struct DataCollector* new_reduced_dc(struct Calibrator* calibrator);

/**
 * Free the datacollector
 * RETURNS:
 *  - DataCollector struct pointer
 * INPUTE:
 *  - NONE
*/
void free_dc(struct DataCollector* datacollector);

/**
 * Add iteration
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - struct DataCollector pointer
 *  - timestep
 *  - value
*/
void add_iteration_dc(struct DataCollector* datacollector, size_t* time_step, int* value);

/**
 * Add gains array
*/
void add_gain_dc(struct DataCollector* datacollector, size_t* time_step, struct array* array);

/**
 * get last gains array
*/
struct Jaggedarray* get_last_gains_dc(struct DataCollector* datacollector);

/**
 * Add all gains captured for a specific timestep in all_gains array
*/
void add_all_gain_dc(struct DataCollector* datacollector, size_t* index, size_t* iteration, struct array* array);

/**
 * Get all gains for all time steps
*/
struct Jaggedarray* get_all_gain_dc(struct DataCollector* datacollector);

/**
 * Adds array of i damping factors to daming_g array at timestep t
 */
void add_damping_g_dc(struct DataCollector* datacollector, size_t* index, struct array* array);

/**
 * Get damping g
 */
struct Jaggedarray* get_damping_g_dc(struct DataCollector* datacollector);


/**
 * Adds array of i damping factors to daming_g_old array at timestep t
 */
void add_damping_g_old_dc(struct DataCollector* datacollector, size_t* index, struct array* array);

/**
 * Get damping g old
 */
struct Jaggedarray* get_damping_g_old_dc(struct DataCollector* datacollector);

/**
 * Get average iterations
 * RETURNS:
 *  - Average Iterations
 * INPUTS:
 *  - struct DataCollector pointer
*/
double* get_average_iterations_dc(struct DataCollector* datacollector);

/**
 * This function returns the iterations array
*/
struct array* get_iterations_dc(struct DataCollector* datacollector);

/**
 * This function will lock the datacollector when used in parallelization
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - struct DataCollector pointer
*/
void lock_dc(struct DataCollector* datacollector);

/**
 * This function will unlock the datacollector when used in parallelization
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - struct DataCollector pointer
*/
void unlock_dc(struct DataCollector* datacollector);

/**
 * Get standard deviation of iterations
 * RETURNS:
 *  - Standard deviation of Iterations
 * INPUTS:
 *  - struct DataCollector pointer
*/
double* get_standard_deviation_iterations_dc(struct DataCollector* datacollector);

/**
 * Add last residual array
 * This will make a copy of the given array and store it in the corrosponding arrays array
*/
void add_last_residual_dc(struct DataCollector* datacollector, size_t* time_step, struct array* array);

/**
 * Get last residual array
*/
struct Jaggedarray* get_last_residual_dc(struct DataCollector* datacollector);

/**
 * Get average of the last residuals
 * The value should be freed
*/
void* get_last_residuals_norm_dc(struct DataCollector* datacollector);

#endif