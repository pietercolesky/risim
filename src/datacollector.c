/**
  * @file datacollector.c
  * This is the implementation of the functions of datacollector.h
  */

#include <stdio.h>
#include <stdlib.h>
#include "datacollector.h"
#include "array.h"
#include "calibrator.h"
#include "utils.h"
#include <time.h>
#include "logger.h"
#include "constants.h"

/**
 * Create new datacollector
 * RETURNS:
 *  - DataCollector struct pointer
 * INPUTE:
 *  - NONE
*/
struct DataCollector* new_dc(struct Calibrator* calibrator) {
    struct DataCollector* datacollector = malloc(sizeof(struct DataCollector));
    if (datacollector == NULL) {
        log_mem_aloc_fail(__FILE__, __LINE__);
    }
    datacollector->calibrator = calibrator;
    datacollector->iterations = zeros_a(&(size_t){1}, (size_t[]){*calibrator->n_steps}, &(enum DataType){INT});
    if (datacollector->iterations == NULL) {
        free_dc(datacollector);
        log_mem_aloc_fail(__FILE__, __LINE__);
    }

    datacollector->gains = new_ja(calibrator->n_steps);
    if (datacollector->gains == NULL) {
        free_dc(datacollector);
        log_mem_aloc_fail(__FILE__, __LINE__);
    }


    if (ALL_DATA) {
        datacollector->all_gains = new_ja(&(size_t){RANDOM_TIMESTEP_AMOUNT * MAX_ITERATIONS_CAPTURE});
        if (datacollector->all_gains == NULL) {
            free_dc(datacollector);
            log_mem_aloc_fail(__FILE__, __LINE__);
        }

        datacollector->damping_g = new_ja(&(size_t){RANDOM_TIMESTEP_AMOUNT});
        if (datacollector->damping_g == NULL) {
            free_dc(datacollector);
            log_mem_aloc_fail(__FILE__, __LINE__);
        }

        datacollector->damping_g_old = new_ja(&(size_t){RANDOM_TIMESTEP_AMOUNT});
        if (datacollector->damping_g_old == NULL) {
            free_dc(datacollector);
            log_mem_aloc_fail(__FILE__, __LINE__);
        }
    }

    datacollector->mutex = malloc(sizeof(pthread_mutex_t));
    if (datacollector->mutex == NULL) {
        free_dc(datacollector);
        log_mem_aloc_fail(__FILE__, __LINE__);
    }
    if (pthread_mutex_init(datacollector->mutex, NULL) != 0) {
        free_dc(datacollector);
        log_mem_aloc_fail(__FILE__, __LINE__);
    }

    datacollector->last_residual = new_ja(calibrator->n_steps);
    if (datacollector->gains == NULL) {
        free_dc(datacollector);
        log_mem_aloc_fail(__FILE__, __LINE__);
    }

    return datacollector;
}

/**
 * Create new datacollector for reduced data collection
 * RETURNS:
 *  - DataCollector struct pointer
 * INPUTE:
 *  - NONE
*/
struct DataCollector* new_reduced_dc(struct Calibrator* calibrator) {
    struct DataCollector* datacollector = malloc(sizeof(struct DataCollector));
    if (datacollector == NULL) {
        log_mem_aloc_fail(__FILE__, __LINE__);
    }
    datacollector->calibrator = calibrator;
    datacollector->iterations = zeros_a(&(size_t){1}, (size_t[]){RANDOM_TIMESTEP_REDUCED_AMOUNT}, &(enum DataType){INT});
    if (datacollector->iterations == NULL) {
        free_dc(datacollector);
        log_mem_aloc_fail(__FILE__, __LINE__);
    }

    datacollector->gains = new_ja(&(size_t){RANDOM_TIMESTEP_REDUCED_AMOUNT});
    if (datacollector->gains == NULL) {
        free_dc(datacollector);
        log_mem_aloc_fail(__FILE__, __LINE__);
    }

    if (ALL_DATA) {
        datacollector->all_gains = new_ja(&(size_t){RANDOM_TIMESTEP_REDUCED_AMOUNT * MAX_ITERATIONS_CAPTURE});
        if (datacollector->all_gains == NULL) {
            free_dc(datacollector);
            log_mem_aloc_fail(__FILE__, __LINE__);
        }

        datacollector->damping_g = new_ja(&(size_t){RANDOM_TIMESTEP_REDUCED_AMOUNT});
        if (datacollector->damping_g == NULL) {
            free_dc(datacollector);
            log_mem_aloc_fail(__FILE__, __LINE__);
        }

        datacollector->damping_g_old = new_ja(&(size_t){RANDOM_TIMESTEP_REDUCED_AMOUNT});
        if (datacollector->damping_g_old == NULL) {
            free_dc(datacollector);
            log_mem_aloc_fail(__FILE__, __LINE__);
        }
    }

    datacollector->mutex = malloc(sizeof(pthread_mutex_t));
    if (datacollector->mutex == NULL) {
        free_dc(datacollector);
        log_mem_aloc_fail(__FILE__, __LINE__);
    }
    if (pthread_mutex_init(datacollector->mutex, NULL) != 0) {
        free_dc(datacollector);
        log_mem_aloc_fail(__FILE__, __LINE__);
    }

    datacollector->last_residual = new_ja(&(size_t){RANDOM_TIMESTEP_REDUCED_AMOUNT});
    if (datacollector->gains == NULL) {
        free_dc(datacollector);
        log_mem_aloc_fail(__FILE__, __LINE__);
    }

    return datacollector;
}

/**
 * Free the datacollector
 * RETURNS:
 *  - DataCollector struct pointer
 * INPUTE:
 *  - NONE
*/
void free_dc(struct DataCollector* datacollector) {
    if (datacollector == NULL) {
        return;
    }

    if (datacollector->iterations != NULL) {
        free_a(datacollector->iterations);
        datacollector->iterations = NULL;
    }

    free_ja(datacollector->gains);

    if (ALL_DATA) {
        free_ja(datacollector->all_gains);
        free_ja(datacollector->damping_g);
        free_ja(datacollector->damping_g_old);
    }

    datacollector->calibrator = NULL;

    if (datacollector->mutex != NULL) {
        pthread_mutex_destroy(datacollector->mutex);
        free(datacollector->mutex);
        datacollector->mutex = NULL;
    }

    free_ja(datacollector->last_residual);

    free(datacollector);
}

/**
 * Add iteration
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - struct DataCollector pointer
 *  - timestep
 *  - value
*/
void add_iteration_dc(struct DataCollector* datacollector, size_t* time_step, int* value) {
    set_element_a(datacollector->iterations, (size_t[]){*time_step}, value);
}

/**
 * Add gains array
 * This will make a copy of the given array and store it in the corrosponding arrays array
*/
void add_gain_dc(struct DataCollector* datacollector, size_t* time_step, struct array* array) {
    set_ja(datacollector->gains, array, time_step);
}

/**
 * get last gains array
*/
struct Jaggedarray* get_last_gains_dc(struct DataCollector* datacollector) {
    return datacollector->gains;
}

/**
 * Add all gains captured for a specific timestep in all_gains array
*/
void add_all_gain_dc(struct DataCollector* datacollector, size_t* index, size_t* iteration, struct array* array) {
    size_t pos = (*index * MAX_ITERATIONS_CAPTURE) + *iteration;
    set_ja(datacollector->all_gains, array, &pos);
}

/**
 * Get all gains for all time steps
*/
struct Jaggedarray* get_all_gain_dc(struct DataCollector* datacollector) {
    size_t new_size = 0;

    struct array* iterations = datacollector->iterations;

    for (size_t i = 0; i < RANDOM_TIMESTEP_AMOUNT; i++) {
        int index = *(int*)get_element_a(datacollector->calibrator->random_timesteps, (size_t[]){i});
        int timestep_amount = *(int*)get_element_a(iterations, (size_t[]){index});

        if (timestep_amount > MAX_ITERATIONS_CAPTURE) {
            timestep_amount = MAX_ITERATIONS_CAPTURE;
        }

        new_size += timestep_amount;
    }

    struct Jaggedarray* resized_all_gains = new_ja(&(size_t){new_size});

    size_t count = 0;
    for (size_t i = 0; i < RANDOM_TIMESTEP_AMOUNT; i++) {
        int index = *(int*)get_element_a(datacollector->calibrator->random_timesteps, (size_t[]){i});
        int timestep_amount = *(int*)get_element_a(iterations, (size_t[]){index});

        if (timestep_amount > MAX_ITERATIONS_CAPTURE) {
            timestep_amount = MAX_ITERATIONS_CAPTURE;
        }

        for (int j = 0; j < timestep_amount; j++) {
            int index_all_gain = i * MAX_ITERATIONS_CAPTURE + j;
            struct array* element_all_gain = get_ja(datacollector->all_gains, (size_t[]){index_all_gain});
            set_ja(resized_all_gains, element_all_gain, (size_t[]){count});
            count += 1;
        }
    }

    return resized_all_gains;
}

/**
 * Adds array of i damping factors to daming_g array at timestep t
 */
void add_damping_g_dc(struct DataCollector* datacollector, size_t* index, struct array* array) {
    set_ja(datacollector->damping_g, array, index);
}

/**
 * Get damping g
 */
struct Jaggedarray* get_damping_g_dc(struct DataCollector* datacollector) {
    return datacollector->damping_g;
}

/**
 * Adds array of i damping factors to daming_g_old array at timestep t
 */
void add_damping_g_old_dc(struct DataCollector* datacollector, size_t* index, struct array* array) {
    set_ja(datacollector->damping_g_old, array, index);
}

/**
 * Get damping g old
 */
struct Jaggedarray* get_damping_g_old_dc(struct DataCollector* datacollector) {
    return datacollector->damping_g_old;
}

/**
 * Get average iterations
 * RETURNS:
 *  - Average Iterations
 * INPUTS:
 *  - struct DataCollector pointer
*/
double* get_average_iterations_dc(struct DataCollector* datacollector) {
    double* mean = (double*)mean_a(datacollector->iterations);
    return mean;
}

/**
 * This function returns the iterations array
*/
struct array* get_iterations_dc(struct DataCollector* datacollector) {
    return datacollector->iterations;
}

/**
 * This function will lock the datacollector when used in parallelization
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - struct DataCollector pointer
*/
void lock_dc(struct DataCollector* datacollector) {
    pthread_mutex_lock(datacollector->mutex);
}

/**
 * This function will unlock the datacollector when used in parallelization
 * RETURNS:
 *  - NONE
 * INPUTS:
 *  - struct DataCollector pointer
*/
void unlock_dc(struct DataCollector* datacollector) {
    pthread_mutex_unlock(datacollector->mutex);
} 

/**
 * Get standard deviation of iterations
 * RETURNS:
 *  - Standard deviation of Iterations
 * INPUTS:
 *  - struct DataCollector pointer
*/
double* get_standard_deviation_iterations_dc(struct DataCollector* datacollector) {
    double* std = (double*)std_a(datacollector->iterations);
    return std;
}

/**
 * Add last residual array
 * This will make a copy of the given array and store it in the corrosponding arrays array
*/
void add_last_residual_dc(struct DataCollector* datacollector, size_t* time_step, struct array* array) {
    set_ja(datacollector->last_residual, array, time_step);
}

/**
 * Get last residual array
*/
struct Jaggedarray* get_last_residual_dc(struct DataCollector* datacollector) {
    return datacollector->last_residual;
}


/**
 * Get average of the last residuals
 * The value should be freed
*/
void* get_last_residuals_norm_dc(struct DataCollector* datacollector) {
    struct Jaggedarray* jarray = datacollector->last_residual;
    if (jarray == NULL) {
        log_null_value(__FILE__, __LINE__);
    }

    switch (*jarray->jarray[0]->type) {
        case DOUBLE: {
            double* norm = norm_ja(jarray);
            return norm;
            break;
        }
        case INT: {
            double* norm = norm_ja(jarray);
            return norm;
            break;
        }
        case FLOAT: {
            float* norm = norm_ja(jarray);
            return norm;
            break;
        }
        case COMPLEX: {
            double complex* norm = norm_ja(jarray);
            return norm;
            break;
        }
        default: {
            log_invalid_type(__FILE__, __LINE__);
        }
    }
    return NULL;
}
