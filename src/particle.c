/**
  * @file particle.c
  * This is the implementation of the functions of particle.h
  */

#include <stdio.h>
#include <stdlib.h>
#include "particle.h"
#include "array.h"
#include "utils.h"
#include "complex.h"
#include <math.h>
#include "logger.h"
#include <stdbool.h>
#include "pso.h"
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "constants.h"

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
struct Particle* new_particle(struct PSO* pso, size_t* num_dimensions, struct array* positions, struct array* velocities) {
    struct Particle* particle = malloc(sizeof(struct Particle));

    //The pso in particle should never be freed with the particle itself.
    particle->pso = pso;
    particle->num_dimensions = malloc(sizeof(size_t));
    *particle->num_dimensions = *num_dimensions;
    particle->w = malloc(sizeof(double));
    *particle->w = 0;
    particle->c1 = malloc(sizeof(double));
    *particle->c1 = 0;
    particle->c2 = malloc(sizeof(double));
    *particle->c2 = 0;
    particle->positions = zeros_a(&(size_t){1}, (size_t[]){*num_dimensions}, &(enum DataType){DOUBLE});
    copy_over_a(particle->positions, positions, (int[]){-1}, (int[]){-1});
    particle->pb_positions = zeros_a(&(size_t){1}, (size_t[]){*num_dimensions}, &(enum DataType){DOUBLE});
    copy_over_a(particle->pb_positions, positions, (int[]){-1}, (int[]){-1});
    particle->velocities = zeros_a(&(size_t){1}, (size_t[]){*num_dimensions}, &(enum DataType){DOUBLE});
    copy_over_a(particle->velocities, velocities, (int[]){-1}, (int[]){-1});
    particle->pb_velocities = zeros_a(&(size_t){1}, (size_t[]){*num_dimensions}, &(enum DataType){DOUBLE});
    copy_over_a(particle->pb_velocities, velocities, (int[]){-1}, (int[]){-1});

    particle->current_residual_norm = malloc(sizeof(double));
    *particle->current_residual_norm = INFINITY;
    particle->current_iterations_mean = malloc(sizeof(double));
    *particle->current_iterations_mean = INFINITY;
    particle->current_iterations_std = malloc(sizeof(double));
    *particle->current_iterations_std = INFINITY;

    particle->pb_residual_norm = malloc(sizeof(double));
    *particle->pb_residual_norm = INFINITY;
    particle->pb_iterations_mean = malloc(sizeof(double));
    *particle->pb_iterations_mean = INFINITY;
    particle->pb_iterations_std = malloc(sizeof(double));
    *particle->pb_iterations_std = INFINITY;
    particle->k = malloc(sizeof(size_t));
    *particle->k = 0;

    //setting w, c1 and c2
    update_hyperparameters_particle(particle);

    return particle;
}

/**
 * Free Particle
 * RETURNS:
 *  - None
 * INPUTE:
 *  - struct Particle pointer
 * 
 * The pso in particle should never be freed with the particle itself.
*/
void free_particle(struct Particle* particle) {
    if (particle == NULL) {
      log_null_value(__FILE__, __LINE__);
    }

    if (particle->num_dimensions != NULL) {
      free(particle->num_dimensions);
    }
    if (particle->w != NULL) {
      free(particle->w);
    }
    if (particle->c1 != NULL) {
      free(particle->c1);
    }
    if (particle->c2 != NULL) {
      free(particle->c2);
    }
    if (particle->positions != NULL) {
      free_a(particle->positions);
    }
    if (particle->velocities != NULL) {
      free_a(particle->velocities);
    }
    if (particle->pb_positions != NULL) {
      free_a(particle->pb_positions);
    }
    if (particle->pb_velocities != NULL) {
      free_a(particle->pb_velocities);
    }
    if (particle->current_residual_norm != NULL) {
      free(particle->current_residual_norm);
    }
    if (particle->current_iterations_mean != NULL) {
      free(particle->current_iterations_mean);
    }
    if (particle->current_iterations_std != NULL) {
      free(particle->current_iterations_std);
    }
    if (particle->pb_residual_norm != NULL) {
      free(particle->pb_residual_norm);
    }
    if (particle->pb_iterations_mean != NULL) {
      free(particle->pb_iterations_mean);
    }
    if (particle->pb_iterations_std != NULL) {
      free(particle->pb_iterations_std);
    }
    if (particle->k != NULL) {
      free(particle->k);
    }

    free(particle);

}

/**
 * Update position
 * RETURNS:
 *  - None
 * INPUTE:
 *  - dimension
 *  - position
*/
void update_position_particle(struct Particle* particle) {
    if (particle == NULL) {
      log_null_value(__FILE__, __LINE__);
    }

    for (size_t d = 0; d < *particle->num_dimensions; d++) {
      double position = *(double*)get_element_a(particle->positions, (size_t[]){d}) + *(double*)get_element_a(particle->velocities, (size_t[]){d});
      set_element_a(particle->positions, (size_t[]){d}, &(double){position});
    }
}

/**
 * Update position Bound
 * RETURNS:
 *  - None
 * INPUTE:
 *  - dimension
 *  - position
*/
void update_position_bounds_particle(struct Particle* particle) {
    if (particle == NULL) {
      log_null_value(__FILE__, __LINE__);
    }

    if (*particle->num_dimensions != 2 && *particle->num_dimensions != 3 ) {
      log_invalid_dimensions(__FILE__, __LINE__);
    }

    double position_x = 0;
    double position_y = 0;
    double position_z = 0;

    for (size_t d = 0; d < *particle->num_dimensions; d++) {
      if (d == 0) {
        double position = *(double*)get_element_a(particle->positions, (size_t[]){d}) + *(double*)get_element_a(particle->velocities, (size_t[]){d});
        position_x = position;
      }
      if (d == 1) {
        double position = *(double*)get_element_a(particle->positions, (size_t[]){d}) + *(double*)get_element_a(particle->velocities, (size_t[]){d});
        position_y = position;
      }
      if (d == 2) {
        double position = *(double*)get_element_a(particle->positions, (size_t[]){d}) + *(double*)get_element_a(particle->velocities, (size_t[]){d});
        position_z = position;
      }
    }

    //check if position is in bounds
    double x_bound = - position_y + 0.5;
    double y_bound = - position_x + 0.5;

    switch (*particle->pso->option) {
      case STEFCAL_LAMBDA:
        if ((position_x >= x_bound || position_y >= y_bound) && 
            (position_x <= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){1})) && 
            (position_y <= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){3})) && 
            (position_x >= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){0})) && 
            (position_y >= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){2}))) 
        {
          //if so update
          set_element_a(particle->positions, (size_t[]){0}, &(double){position_x});
          set_element_a(particle->positions, (size_t[]){1}, &(double){position_y});
        }
        break;
      case LEVENBERG_MARQUARDT_NON_ADAPTIVE_L1_L2:
        if ((position_x >= x_bound || position_y >= y_bound) && 
            (position_x <= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){1})) && 
            (position_y <= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){3})) && 
            (position_x >= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){0})) && 
            (position_y >= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){2}))) 
        {
          //if so update
          set_element_a(particle->positions, (size_t[]){0}, &(double){position_x});
          set_element_a(particle->positions, (size_t[]){1}, &(double){position_y});
        }
        break;
      case LEVENBERG_MARQUARDT:
        if ((position_x <= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){1})) && 
            (position_y <= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){3})) && 
            (position_x >= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){0})) && 
            (position_y >= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){2}))) 
        {
          //if so update
          set_element_a(particle->positions, (size_t[]){0}, &(double){position_x});
          set_element_a(particle->positions, (size_t[]){1}, &(double){position_y});
        }
        break;
      case STEFCAL_IMPROVED:
        if ((position_x >= x_bound || position_y >= y_bound) && 
            (position_x <= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){1})) && 
            (position_y <= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){3})) && 
            (position_x >= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){0})) && 
            (position_y >= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){2})) &&
            (position_x <= -0.05 || position_x >= 0.05) &&
            (position_y <= -0.05 || position_y >= 0.05)) 
        {
          //if so update
          set_element_a(particle->positions, (size_t[]){0}, &(double){position_x});
          set_element_a(particle->positions, (size_t[]){1}, &(double){position_y});
        }
        break;
      case LEVENBERG_MARQUARDT_REDUCED_TIMESTEPS:
        if ((position_x <= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){1})) && 
            (position_y <= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){3})) && 
            (position_x >= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){0})) && 
            (position_y >= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){2}))) 
        {
          //if so update
          set_element_a(particle->positions, (size_t[]){0}, &(double){position_x});
          set_element_a(particle->positions, (size_t[]){1}, &(double){position_y});
        }
        break;
      case LEVENBERG_MARQUARDT_NON_ADAPTIVE_L1_L2_REDUCED_TIMESTEPS:
        if ((position_x >= x_bound || position_y >= y_bound) && 
            (position_x <= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){1})) && 
            (position_y <= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){3})) && 
            (position_x >= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){0})) && 
            (position_y >= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){2}))) 
        {
          //if so update
          set_element_a(particle->positions, (size_t[]){0}, &(double){position_x});
          set_element_a(particle->positions, (size_t[]){1}, &(double){position_y});
        }
        break;
      case ADAM:
        if ((position_x <= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){1})) && 
            (position_x >= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){0})) && 
            (position_y <= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){3})) && 
            (position_y >= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){2})) &&
            (position_z <= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){5})) && 
            (position_z >= *(double*)get_element_a(particle->pso->position_bounds, (size_t[]){4}))) 
        {
          //if so update
          set_element_a(particle->positions, (size_t[]){0}, &(double){position_x});
          set_element_a(particle->positions, (size_t[]){1}, &(double){position_y});
          set_element_a(particle->positions, (size_t[]){2}, &(double){position_z});
        }
        break;
      default:
        break;
    }
}

/**
 * Update velocity
 * RETURNS:
 *  - None
 * INPUTE:
 *  - dimension
 *  - velocity
*/
void update_velocity_particle(struct Particle* particle) {
    if (particle == NULL) {
      log_null_value(__FILE__, __LINE__);
    }

    for (size_t d = 0; d < *particle->num_dimensions; d++) {
      double r1 = gsl_rng_uniform(particle->pso->rng);
      double r2 = gsl_rng_uniform(particle->pso->rng);
      double velocity = *particle->w * *(double*)get_element_a(particle->velocities, (size_t[]){d}) +  
                        *particle->c1 * r1 * (*(double*)get_element_a(particle->pb_positions, (size_t[]){d}) - *(double*)get_element_a(particle->positions, (size_t[]){d})) +  
                        *particle->c2 * r2 * (*(double*)get_element_a(particle->pso->global_best_positions, (size_t[]){d}) - *(double*)get_element_a(particle->positions, (size_t[]){d}));
      set_element_a(particle->velocities, (size_t[]){d}, &(double){velocity});
    }
}

/**
 * Update hyperparameters
 * RETURNS:
 *  - None
 * INPUTE:
 *  - struct Particle pointer
*/
void update_hyperparameters_particle(struct Particle* particle) {
    if (particle == NULL) {
      log_null_value(__FILE__, __LINE__);
    }

    // w = random number between -0.9 and 0.9
    double w = gsl_rng_uniform(particle->pso->rng) * 1.8 - 0.9;
    double lower_bound = (22-(30 * pow(w, 2))) / (7-5 * w);
    double upper_bound = (24-(24 * pow(w, 2))) / (7-5 * w);

    if (lower_bound < 0.2) {
      lower_bound = 0.2;
    }
    if (upper_bound < 0.2) {
      upper_bound = 0.2;
    }

    double random_uniform = gsl_rng_uniform(particle->pso->rng);
    double c1_c2 = lower_bound + random_uniform * (upper_bound - lower_bound); 

    int choice = gsl_rng_uniform_int(particle->pso->rng, 3);

    if (choice == 0) {
      //Equal
      *particle->w = w;
      *particle->c1 = (1.0/2.0) * c1_c2;
      *particle->c2 = (1.0/2.0) * c1_c2;

    } else if (choice == 1) {
      //Cognitive
      *particle->w = w;
      *particle->c1 = (1.0/4.0) * c1_c2;
      *particle->c2 = (3.0/4.0) * c1_c2;

    } else if (choice == 2) {
      //Social
      *particle->w = w;
      *particle->c1 = (3.0/4.0) * c1_c2;
      *particle->c2 = (1.0/4.0) * c1_c2;

    } else {
      log_invalid_choice(__FILE__, __LINE__);
    }

}

/**
 * Check if hyperparameters needs to be updated
 * RETURNS:
 *  - None
 * INPUTE:
 *  - struct Particle pointer
*/
void check_update_hyperparameters_particle(struct Particle* particle, size_t* k) {
    if (particle == NULL) {
      log_null_value(__FILE__, __LINE__);
    }

    if (*particle->k >= *k) {
      //need to update the particle and set k to 0
      update_hyperparameters_particle(particle);
      *particle->k = 0;
    }
}

/**
 * Update fitness
 * This function will also update the pb_position and pb velocity
 * RETURNS:
 *  - None
 * INPUTE:
 *  - pb_residual_norm
 *  - pb_iterations_mean
 *  - pb_iterations_std
*/
void update_fitness_particle(struct Particle* particle) {
    if (particle == NULL) {
      log_null_value(__FILE__, __LINE__);
    }
    copy_over_a(particle->pb_positions, particle->positions, (int[]){-1,-1,-1}, (int[]){-1,-1,-1});
    *particle->pb_residual_norm = *particle->current_residual_norm;
    *particle->pb_iterations_mean = *particle->current_iterations_mean;
    *particle->pb_iterations_std = *particle->current_iterations_std;
}

/**
 * Check if fitness needs to be updated
 * RETURNS:
 *  - None
 * INPUTE:
 *  - struct Particle pointer
*/
void check_update_fitness_particle(struct Particle* particle) {
    if (particle == NULL) {
      log_null_value(__FILE__, __LINE__);
    }

    bool check = true;
    if (*particle->current_residual_norm != NAN && *particle->current_residual_norm != INFINITY) {
      if (*particle->current_iterations_mean < *particle->pb_iterations_mean) {
        update_fitness_particle(particle);
        check = false;
      } else if (*particle->current_iterations_mean == *particle->pb_iterations_mean) {
        //when equal check if the iterations mean is less
        if (fabs(*particle->current_iterations_std - *particle->pb_iterations_std) > EPSILON_PSO &&
                *particle->current_iterations_std < *particle->pb_iterations_std) {
          //update fitness
          update_fitness_particle(particle);
          check = false;
        } else if (fabs(*particle->current_iterations_std - *particle->pb_iterations_std) <= EPSILON_PSO) {
          if (fabs(*particle->current_residual_norm - *particle->pb_residual_norm) > EPSILON_PSO &&
                  *particle->current_residual_norm < *particle->pb_residual_norm) {
            update_fitness_particle(particle);
            check = false;
          }
        }
      }
    }

    if (check) {
      *particle->k += 1;
    }
}

