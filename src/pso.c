/**
  * @file pso.c
  * This is the implementation of the functions of pso.h
  */

#include <stdio.h>
#include <stdlib.h>
#include "pso.h"
#include <gsl/gsl_rng.h>
#include "logger.h"
#include <math.h>
#include "particle.h"
#include "constants.h"
#include "simulator.h"
#include "calibrator.h"
#include "datacollector.h"
#include "string.h"
#include "data_dictionary.h"
#include "data_container.h"
#include "pso_progress.h"

/**
 * Create new pso
 * RETURNS:
 *  - PSO struct pointer
 * INPUTE:
 * 
*/
struct PSO* new_pso(enum AlgorithmType* option, size_t* num_dimensions, struct array* position_bounds, size_t* iterations_max, size_t* num_particles, int* seed, char* simulators_directory, int* order, size_t* k, char* dd_name, char* dd_directory,int* num_workers) {
    struct PSO* pso = malloc(sizeof(struct PSO));

    pso->option = malloc(sizeof(enum AlgorithmType));
    *pso->option = *option;
    //Number of dimensions
    pso->num_dimensions = malloc(sizeof(size_t));
    *pso->num_dimensions = *num_dimensions;
    //Global best residual mean
    pso->global_best_residual_norm = malloc(sizeof(double));
    *pso->global_best_residual_norm = INFINITY;
    pso->global_best_iteration_mean = malloc(sizeof(double));
    *pso->global_best_iteration_mean = INFINITY;
    pso->global_best_iteration_std = malloc(sizeof(double));
    *pso->global_best_iteration_std = INFINITY;
    pso->position_bounds = zeros_a(position_bounds->num_dimensions, position_bounds->dimensions, position_bounds->type);
    copy_over_a(pso->position_bounds, position_bounds, (int[]){-1}, (int[]){-1});
    pso->init_velocities = zeros_a(&(size_t){1}, num_dimensions, &(enum DataType){DOUBLE});
    pso->global_best_w = malloc(sizeof(double));
    *pso->global_best_w = INFINITY;
    pso->global_best_c1 = malloc(sizeof(double));
    *pso->global_best_c1 = INFINITY;
    pso->global_best_c2 = malloc(sizeof(double));
    *pso->global_best_c2 = INFINITY;
    pso->iterations_max = malloc(sizeof(size_t));
    *pso->iterations_max = *iterations_max;
    pso->num_particles = malloc(sizeof(size_t));
    *pso->num_particles = *num_particles;
    pso->seed = malloc(sizeof(int));
    *pso->seed = *seed;
    //setting random number generator
    const gsl_rng_type *rng_type;
    gsl_rng_env_setup();
    rng_type = gsl_rng_default;
    pso->rng = gsl_rng_alloc(rng_type);
    gsl_rng_set(pso->rng, *seed);

    pso->mutex = malloc(sizeof(pthread_mutex_t));

    pso->simulators_directory =  (char*) malloc(strlen(simulators_directory) + 1);
    if (pso->simulators_directory == NULL) {
        log_mem_aloc_fail(__FILE__, __LINE__);
    }
    strcpy(pso->simulators_directory, simulators_directory);
    pso->order = malloc(sizeof(int));
    *pso->order = *order;

    pso->k = malloc(sizeof(size_t));
    *pso->k = *k;

    struct array* global_best_positions = zeros_a(&(size_t){1}, num_dimensions, &(enum DataType){DOUBLE});
    for (size_t d = 0; d < *num_dimensions; d++) {
      set_element_a(global_best_positions, (size_t[]){d}, &(double){INFINITY});
    }
    pso->global_best_positions = global_best_positions;

    pso->dd_name =  (char*) malloc(strlen(dd_name) + 1);
    if (pso->dd_name == NULL) {
        log_mem_aloc_fail(__FILE__, __LINE__);
    }
    strcpy(pso->dd_name, dd_name);

    pso->dd_directory =  (char*) malloc(strlen(dd_directory) + 1);
    if (pso->dd_directory == NULL) {
        log_mem_aloc_fail(__FILE__, __LINE__);
    }
    strcpy(pso->dd_directory, dd_directory);
    pso->num_workers = malloc(sizeof(int));
    *pso->num_workers = *num_workers;

    return pso;
}

/**
 * Free pso
 * RETURNS:
 * - None
 * INPUTE:
 * - PSO struct pointer
*/
void free_pso(struct PSO* pso) {
    if (pso == NULL) {
      log_null_value(__FILE__, __LINE__);
    }

    if (pso->option != NULL) {
      free(pso->option);
    }
    if (pso->num_dimensions != NULL) {
      free(pso->num_dimensions);
    }
    if (pso->global_best_residual_norm != NULL) {
      free(pso->global_best_residual_norm);
    }
    if (pso->global_best_iteration_mean != NULL) {
      free(pso->global_best_iteration_mean);
    }
    if (pso->global_best_iteration_std != NULL) {
      free(pso->global_best_iteration_std);
    }
    if (pso->position_bounds != NULL) {
      free_a(pso->position_bounds);
    }
    if (pso->init_velocities != NULL) {
      free_a(pso->init_velocities);
    }
    if (pso->global_best_w != NULL) {
      free(pso->global_best_w);
    }
    if (pso->global_best_c1 != NULL) {
      free(pso->global_best_c1);
    }
    if (pso->global_best_c2 != NULL) {
      free(pso->global_best_c2);
    }
    if (pso->iterations_max != NULL) {
      free(pso->iterations_max);
    }
    if (pso->num_particles != NULL) {
      free(pso->num_particles);
    }
    if (pso->seed != NULL) {
      free(pso->seed);
    }
    if (pso->rng != NULL) {
      gsl_rng_free(pso->rng);
    }
    if (pso->mutex != NULL) {
      // pthread_mutex_destroy(pso->mutex);
      free(pso->mutex);
    }
    if (pso->simulators_directory != NULL) {
      free(pso->simulators_directory);
    }
    if (pso->order != NULL) {
      free(pso->order);
    }
    if (pso->k != NULL) {
      free(pso->k);
    }
    if (pso->global_best_positions != NULL) {
      free_a(pso->global_best_positions);
    }
    if (pso->dd_name != NULL) {
      free(pso->dd_name);
    }
    if (pso->dd_directory != NULL) {
      free(pso->dd_directory);
    }
    if (pso->num_workers != NULL) {
      free(pso->num_workers);
    }

    free(pso);
}

/**
 * Free pso
 * RETURNS:
 * - None
 * INPUTE:
 * - PSO struct pointer
*/
void start_pso(struct PSO* pso) {

    if (pso == NULL) {
      log_null_value(__FILE__, __LINE__);
    }

    switch (*pso->option)
    {
      case STEFCAL_LAMBDA:
        printf("STEFCAL_LAMBDA\n");
        break;
      case STEFCAL_IMPROVED:
        printf("STEFCAL_IMPROVED\n");
        break;
      case LEVENBERG_MARQUARDT_NON_ADAPTIVE_L1_L2:
        printf("LEVENBERG_MARQUARDT_NON_ADAPTIVE_L1_L2\n");
        break;
      case LEVENBERG_MARQUARDT_NON_ADAPTIVE_L1_L2_REDUCED_TIMESTEPS:
        printf("LEVENBERG_MARQUARDT_NON_ADAPTIVE_L1_L2_REDUCED_TIMESTEPS\n");
        break;  
      case LEVENBERG_MARQUARDT:
        printf("LEVENBERG_MARQUARDT\n");
        break;
      case LEVENBERG_MARQUARDT_REDUCED_TIMESTEPS:
        printf("LEVENBERG_MARQUARDT_REDUCED_TIMESTEPS\n");
        break;
      case ADAM:
        printf("ADAM\n");
        break;
      default:
        log_invalid_algorithm_type(__FILE__, __LINE__);
      break;
    }

    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};

    //Opening DataDictionary and getting size of DataDictionary
    struct DataDictionary* data_dictionary = read_from_file_dd(pso->dd_directory, pso->dd_name);
    size_t dd_count = *(size_t*)data_dictionary->amount_items;
    size_t count = 0;

    struct PSOProgress* pso_progress = new_pp(&(size_t){50}, &(size_t){64}, pso->iterations_max, pso->num_particles);

    //Iterate through all the snrs, max_amps and seeds 
    for (size_t j = 0; j < 4; j++) {
          for (size_t h = 0; h < 4; h++) {
              for (size_t l = 0; l < 4; l++) {
                  if (count >= dd_count) {
                    //Create array of particles
                    struct Particle** particles = malloc(*pso->num_particles * sizeof(struct Particle*));
                    //initialise particle positions and velocities
                    for (size_t p = 0; p < *pso->num_particles; p++) {
                      struct array* position;
                      if (*pso->option == LEVENBERG_MARQUARDT_NON_ADAPTIVE_L1_L2 || *pso->option == STEFCAL_LAMBDA || *pso->option == STEFCAL_LM_LAMBDA) {
                        position = random_position_bound1_pso(pso);
                      } else if (*pso->option == STEFCAL_IMPROVED) {
                        position = random_position_bound2_pso(pso);
                      } else {
                        position = random_position_pso(pso);
                      }
                      struct Particle* particle = new_particle(pso, pso->num_dimensions, position, pso->init_velocities);
                      particles[p] = particle;
                      free_a(position);
                    }

                    int seed = seeds[j];
                    double snr = snrs[h];
                    double max_amp = max_amps[l];
                    char filename [256];
                    snprintf(filename, sizeof(filename), "%s/sim_seed_%u_order_%d_snr_%.0lf_maxamp_%.0lf.sconfig", pso->simulators_directory, seed, *pso->order, snr, max_amp);
                    struct Simulator* simulator = read_from_file_s(filename);
                    struct Calibrator* calibrator = new_c(simulator);
                    set_parallel_c(calibrator, pso->num_workers);
                    //Clear the Global Best variables of the pso
                    reset_pso(pso);
                    //start iterative process of calculating the global best
                    size_t global_best_update_count = 0;

                    //This loop will run until there is convergence or max iteration is achieved
                    for (size_t iter = 0; iter < *pso->iterations_max && global_best_update_count < 10; iter++) {
                      //check and update the w, c1 and c2
                      for (size_t p = 0; p < *pso->num_particles; p++) {
                        check_update_hyperparameters_particle(particles[p], pso->k);
                      }

                      //loop through all particles, run them and and update Global and Personal if needed
                      for (size_t p = 0; p < *pso->num_particles; p++) {
                        double* average_iterations = &(double){0};
                        double* std_iterations = &(double){0};
                        double* last_residual_norm = &(double){0};
                        //check the option of the algorithm
                        switch (*pso->option)
                        {
                        case LEVENBERG_MARQUARDT:
                          solve_G_LM(
                            calibrator, 
                            &(size_t){IMAX}, 
                            &(double){TAU}, 
                            &(double){*(double*)get_element_a(particles[p]->positions, (size_t[]){0})}, 
                            &(double){*(double*)get_element_a(particles[p]->positions, (size_t[]){1})}
                          );
                          //set current values
                          average_iterations = get_average_iterations_dc(calibrator->datacollector);
                          std_iterations = get_standard_deviation_iterations_dc(calibrator->datacollector);
                          last_residual_norm = get_last_residuals_norm_dc(calibrator->datacollector);
                          *particles[p]->current_iterations_mean = *average_iterations;
                          *particles[p]->current_iterations_std = *std_iterations; 
                          *particles[p]->current_residual_norm = *last_residual_norm;
                          free(last_residual_norm);
                          free(std_iterations);
                          free(average_iterations);
                          break;
                        default:
                          log_invalid_algorithm_type(__FILE__, __LINE__);
                          break;
                        }

                        increment_particle_pp(pso_progress);
                      }

                      //update personal best
                      for (size_t p = 0; p < *pso->num_particles; p++) {
                        check_update_fitness_particle(particles[p]);
                      }

                      int global_update = 0;

                      //update global best
                      for (size_t p = 0; p < *pso->num_particles; p++) {
                        if (*particles[p]->current_residual_norm != -NAN && 
                            *particles[p]->current_residual_norm != INFINITY && 
                            *particles[p]->current_residual_norm != NAN &&
                            *particles[p]->current_residual_norm != -INFINITY ) 
                        {
                          if (*particles[p]->pb_iterations_mean < *pso->global_best_iteration_mean) {
                            *pso->global_best_iteration_mean = *particles[p]->pb_iterations_mean;
                            *pso->global_best_iteration_std = *particles[p]->pb_iterations_std;
                            *pso->global_best_residual_norm = *particles[p]->pb_residual_norm;
                            copy_over_a(pso->global_best_positions,particles[p]->pb_positions, (int[]){-1,-1,-1}, (int[]){-1,-1,-1});
                            *pso->global_best_w = *particles[p]->w;
                            *pso->global_best_c1 = *particles[p]->c1;
                            *pso->global_best_c2 = *particles[p]->c2;
                            global_update = 1;
                          } else if (*particles[p]->pb_iterations_mean == *pso->global_best_iteration_mean) {
                            if (fabs(*particles[p]->pb_iterations_std - *pso->global_best_iteration_std) > EPSILON_PSO &&
                                    *particles[p]->pb_iterations_std < *pso->global_best_iteration_std) {
                              *pso->global_best_iteration_mean = *particles[p]->pb_iterations_mean;
                              *pso->global_best_iteration_std = *particles[p]->pb_iterations_std;
                              *pso->global_best_residual_norm = *particles[p]->pb_residual_norm;
                              copy_over_a(pso->global_best_positions,particles[p]->pb_positions, (int[]){-1,-1,-1}, (int[]){-1,-1,-1});
                              *pso->global_best_w = *particles[p]->w;
                              *pso->global_best_c1 = *particles[p]->c1;
                              *pso->global_best_c2 = *particles[p]->c2;
                              global_update = 1;
                            } else if (fabs(*particles[p]->pb_iterations_std - *pso->global_best_iteration_std) <= EPSILON_PSO) {
                              if (fabs(*particles[p]->pb_residual_norm - *pso->global_best_residual_norm) > EPSILON_PSO &&
                                      *particles[p]->pb_residual_norm < *pso->global_best_residual_norm) {
                                *pso->global_best_iteration_mean = *particles[p]->pb_iterations_mean;
                                *pso->global_best_iteration_std = *particles[p]->pb_iterations_std;
                                *pso->global_best_residual_norm = *particles[p]->pb_residual_norm;
                                copy_over_a(pso->global_best_positions,particles[p]->pb_positions, (int[]){-1,-1,-1}, (int[]){-1,-1,-1});
                                *pso->global_best_w = *particles[p]->w;
                                *pso->global_best_c1 = *particles[p]->c1;
                                *pso->global_best_c2 = *particles[p]->c2;
                                global_update = 1;
                              }
                            }
                          }
                        }
                      }

                      //if no update increment 
                      if (global_update) {
                        global_best_update_count = 0;
                      } else {
                        global_best_update_count += 1;
                      }

                      //update velocity
                      for (size_t p = 0; p < *pso->num_particles; p++) {
                        update_velocity_particle(particles[p]);
                      }
                      //update position
                      for (size_t p = 0; p < *pso->num_particles; p++) {
                        update_position_bounds_particle(particles[p]);
                      }

                      increment_iterations_pp(pso_progress);
                    }

                    //saving data to datacontainer
                    char dc_name[256];
                    sprintf(dc_name, "seed_%u_order_%d_snr_%.0lf_maxamp_%.0lf", seeds[j], *pso->order, snrs[h], max_amps[l]); 
                    struct DataContainer* data_container = new_data_container_type_1_update_dd(&data_dictionary, dc_name, &(size_t){2});
                    set_new_array_dcon(&data_container, &(size_t){0}, "positions", pso->global_best_positions);
                    struct array* double_data = new_set_a(&(size_t){1},
                                                          (size_t[]){6}, 
                                                          &(enum DataType){DOUBLE}, 
                                                          (double[6]){
                                                              *pso->global_best_iteration_mean,
                                                              *pso->global_best_iteration_std,
                                                              *pso->global_best_residual_norm,
                                                              *pso->global_best_w,
                                                              *pso->global_best_c1,
                                                              *pso->global_best_c2,
                                                          }); 
                    set_new_array_dcon(&data_container, &(size_t){1}, "double_data", double_data);  
                    free_a(double_data);                                                                                         

                    //save global variables to datacontainer
                    free_dcon(data_container);
                    free_c(calibrator);
                    free_s(simulator);

                    //free all particles
                    for (size_t p = 0; p < *pso->num_particles; p++) {
                      free_particle(particles[p]);
                    }
                    free(particles);

                    increment_pso_pp(pso_progress);
                  } else {
                    increment_pso_no_effect_pp(pso_progress);
                  }
                  count += 1;
              }
          }
      }

      stop_free_pp(pso_progress, "");
      free_dd(data_dictionary);
}

/**
 * Start pso and do a singel run
 * RETURNS:
 * - 0 if no more to do and 1 if there is still stuff to do 
 * INPUTE:
 * - PSO struct pointer
*/
void start_single_pso_local(struct PSO* pso) {
    if (pso == NULL) {
      log_null_value(__FILE__, __LINE__);
    }

    switch (*pso->option)
    {
      case STEFCAL_LAMBDA:
        printf("STEFCAL_LAMBDA\n");
        break;
      case STEFCAL_IMPROVED:
        printf("STEFCAL_IMPROVED\n");
        break;
      case LEVENBERG_MARQUARDT_NON_ADAPTIVE_L1_L2:
        printf("LEVENBERG_MARQUARDT_NON_ADAPTIVE_L1_L2\n");
        break;
      case LEVENBERG_MARQUARDT_NON_ADAPTIVE_L1_L2_REDUCED_TIMESTEPS:
        printf("LEVENBERG_MARQUARDT_NON_ADAPTIVE_L1_L2_REDUCED_TIMESTEPS\n");
        break;  
      case LEVENBERG_MARQUARDT:
        printf("LEVENBERG_MARQUARDT\n");
        break;
      case LEVENBERG_MARQUARDT_REDUCED_TIMESTEPS:
        printf("LEVENBERG_MARQUARDT_REDUCED_TIMESTEPS\n");
        break;
      case ADAM:
        printf("ADAM\n");
        break;
      default:
        log_invalid_algorithm_type(__FILE__, __LINE__);
      break;
    }

    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};

    //Opening DataDictionary and getting size of DataDictionary
    struct DataDictionary* data_dictionary = read_from_file_dd(pso->dd_directory, pso->dd_name);
    size_t dd_count = *(size_t*)data_dictionary->amount_items;
    size_t count = 0;

    struct PSOProgress* pso_progress = new_pp(&(size_t){50}, &(size_t){64}, pso->iterations_max, pso->num_particles);
    
    for (size_t j = 0; j < 4; j++) {
      for (size_t h = 0; h < 4; h++) {
        for (size_t l = 0; l < 4; l++) {
            if (count >= dd_count) {
              //Create array of particles
              struct Particle** particles = malloc(*pso->num_particles * sizeof(struct Particle*));
              //initialise particle positions and velocities
              for (size_t p = 0; p < *pso->num_particles; p++) {
                struct array* position;
                if (*pso->option == LEVENBERG_MARQUARDT_NON_ADAPTIVE_L1_L2 || *pso->option == STEFCAL_LAMBDA || *pso->option == STEFCAL_LM_LAMBDA) {
                  position = random_position_bound1_pso(pso);
                } else if (*pso->option == STEFCAL_IMPROVED) {
                  position = random_position_bound2_pso(pso);
                } else {
                  position = random_position_pso(pso);
                }
                struct Particle* particle = new_particle(pso, pso->num_dimensions, position, pso->init_velocities);
                particles[p] = particle;
                free_a(position);
              }

              int seed = seeds[j];
              double snr = snrs[h];
              double max_amp = max_amps[l];
              char filename [256];
              snprintf(filename, sizeof(filename), "%s/sim_seed_%u_order_%d_snr_%.0lf_maxamp_%.0lf.sconfig", pso->simulators_directory, seed, *pso->order, snr, max_amp);
              struct Simulator* simulator = read_from_file_s(filename);
              struct Calibrator* calibrator = new_c(simulator);
              set_parallel_c(calibrator, pso->num_workers);
              reset_pso(pso);
              size_t global_best_update_count = 0;

              //This loop will run until there is convergence or max iteration is achieved
              for (size_t iter = 0; iter < *pso->iterations_max && global_best_update_count < 10; iter++) {
                //check and update the w, c1 and c2
                for (size_t p = 0; p < *pso->num_particles; p++) {
                  check_update_hyperparameters_particle(particles[p], pso->k);
                }
                //loop through all particles, run them and and update Global and Personal if needed
                for (size_t p = 0; p < *pso->num_particles; p++) {
                  //check the option of the algorithm
                  double* average_iterations = &(double){0};
                  double* std_iterations = &(double){0};
                  double* last_residual_norm = &(double){0};
                  switch (*pso->option)
                  {
                    case LEVENBERG_MARQUARDT:
                      solve_G_LM(
                        calibrator, 
                        &(size_t){IMAX}, 
                        &(double){TAU}, 
                        &(double){*(double*)get_element_a(particles[p]->positions, (size_t[]){0})}, 
                        &(double){*(double*)get_element_a(particles[p]->positions, (size_t[]){1})}
                      );
                      //set current values
                      average_iterations = get_average_iterations_dc(calibrator->datacollector);
                      std_iterations = get_standard_deviation_iterations_dc(calibrator->datacollector);
                      last_residual_norm = get_last_residuals_norm_dc(calibrator->datacollector);
                      *particles[p]->current_iterations_mean = *average_iterations;
                      *particles[p]->current_iterations_std = *std_iterations; 
                      *particles[p]->current_residual_norm = *last_residual_norm;
                      free(last_residual_norm);
                      free(std_iterations);
                      free(average_iterations);
                      break;
                    case LEVENBERG_MARQUARDT_REDUCED_TIMESTEPS:
                      solve_G_LM_REDUCED_TIMESTEPS(
                        calibrator, 
                        &(size_t){IMAX}, 
                        &(double){TAU}, 
                        &(double){*(double*)get_element_a(particles[p]->positions, (size_t[]){0})}, 
                        &(double){*(double*)get_element_a(particles[p]->positions, (size_t[]){1})}
                      );
                      //set current values
                      
                      average_iterations = get_average_iterations_dc(calibrator->datacollector);
                      std_iterations = get_standard_deviation_iterations_dc(calibrator->datacollector);
                      last_residual_norm = get_last_residuals_norm_dc(calibrator->datacollector);

                      *particles[p]->current_iterations_mean = *average_iterations;
                      *particles[p]->current_iterations_std = *std_iterations; 
                      *particles[p]->current_residual_norm = *last_residual_norm;

                      free(last_residual_norm);
                      free(std_iterations);
                      free(average_iterations);
                      break;
                  default:
                    log_invalid_algorithm_type(__FILE__, __LINE__);
                    break;
                  }

                  increment_particle_pp(pso_progress);
                }
                //update personal best
                for (size_t p = 0; p < *pso->num_particles; p++) {
                  check_update_fitness_particle(particles[p]);
                }

                int global_update = 0;
                //update global best
                for (size_t p = 0; p < *pso->num_particles; p++) {
                  if (*particles[p]->current_residual_norm != NAN && *particles[p]->current_residual_norm != INFINITY) {
                    if (*particles[p]->pb_iterations_mean < *pso->global_best_iteration_mean) {
                      *pso->global_best_iteration_mean = *particles[p]->pb_iterations_mean;
                      *pso->global_best_iteration_std = *particles[p]->pb_iterations_std;
                      *pso->global_best_residual_norm = *particles[p]->pb_residual_norm;
                      copy_over_a(pso->global_best_positions,particles[p]->pb_positions, (int[]){-1,-1,-1}, (int[]){-1,-1,-1});
                      *pso->global_best_w = *particles[p]->w;
                      *pso->global_best_c1 = *particles[p]->c1;
                      *pso->global_best_c2 = *particles[p]->c2;
                      global_update = 1;
                    } else if (*particles[p]->pb_iterations_mean == *pso->global_best_iteration_mean) {
                      if (fabs(*particles[p]->pb_iterations_std - *pso->global_best_iteration_std) > EPSILON_PSO &&
                              *particles[p]->pb_iterations_std < *pso->global_best_iteration_std) {
                        *pso->global_best_iteration_mean = *particles[p]->pb_iterations_mean;
                        *pso->global_best_iteration_std = *particles[p]->pb_iterations_std;
                        *pso->global_best_residual_norm = *particles[p]->pb_residual_norm;
                        copy_over_a(pso->global_best_positions,particles[p]->pb_positions, (int[]){-1,-1,-1}, (int[]){-1,-1,-1});
                        *pso->global_best_w = *particles[p]->w;
                        *pso->global_best_c1 = *particles[p]->c1;
                        *pso->global_best_c2 = *particles[p]->c2;
                        global_update = 1;
                      } else if (fabs(*particles[p]->pb_iterations_std - *pso->global_best_iteration_std) <= EPSILON_PSO) {
                        if (fabs(*particles[p]->pb_residual_norm - *pso->global_best_residual_norm) > EPSILON_PSO &&
                                *particles[p]->pb_residual_norm < *pso->global_best_residual_norm) {
                          *pso->global_best_iteration_mean = *particles[p]->pb_iterations_mean;
                          *pso->global_best_iteration_std = *particles[p]->pb_iterations_std;
                          *pso->global_best_residual_norm = *particles[p]->pb_residual_norm;
                          copy_over_a(pso->global_best_positions,particles[p]->pb_positions, (int[]){-1,-1,-1}, (int[]){-1,-1,-1});
                          *pso->global_best_w = *particles[p]->w;
                          *pso->global_best_c1 = *particles[p]->c1;
                          *pso->global_best_c2 = *particles[p]->c2;
                          global_update = 1;
                        }
                      }
                    }
                  }
                }
                //if no update increment 
                if (global_update) {
                  global_best_update_count = 0;
                } else {
                  global_best_update_count += 1;
                }

                //update velocity
                for (size_t p = 0; p < *pso->num_particles; p++) {
                  update_velocity_particle(particles[p]);
                }
                
                //update position
                for (size_t p = 0; p < *pso->num_particles; p++) {
                  update_position_bounds_particle(particles[p]);
                }

                increment_iterations_pp(pso_progress);
              }
              //saving data to datacontainer
              char dc_name[256];
              sprintf(dc_name, "seed_%u_order_%d_snr_%.0lf_maxamp_%.0lf", seeds[j], *pso->order, snrs[h], max_amps[l]);
              struct DataContainer* data_container = new_data_container_type_1_update_dd(&data_dictionary, dc_name, &(size_t){2});
              set_new_array_dcon(&data_container, &(size_t){0}, "positions", pso->global_best_positions);
              struct array* double_data = new_set_a(&(size_t){1},
                                                    (size_t[]){6}, 
                                                    &(enum DataType){DOUBLE}, 
                                                    (double[6]){
                                                        *pso->global_best_iteration_mean,
                                                        *pso->global_best_iteration_std,
                                                        *pso->global_best_residual_norm,
                                                        *pso->global_best_w,
                                                        *pso->global_best_c1,
                                                        *pso->global_best_c2,
                                                    }); 
              set_new_array_dcon(&data_container, &(size_t){1}, "double_data", double_data);  
              free_a(double_data);                                                                                       

              //save global variables to datacontainer
              free_dcon(data_container);
              free_c(calibrator);
              free_s(simulator);

              //free all particles
              for (size_t p = 0; p < *pso->num_particles; p++) {
                free_particle(particles[p]);
              }
              free(particles);

              increment_pso_pp(pso_progress);
            } else {
              increment_pso_no_effect_pp(pso_progress);
            }
            count += 1;
        }
      }
    }
    
    stop_free_pp(pso_progress, "");
    free_dd(data_dictionary);

}

/**
 * Start pso and do a singel run
 * RETURNS:
 * - 0 if no more to do and 1 if there is still stuff to do 
 * INPUTE:
 * - PSO struct pointer
*/
int start_single_pso(struct PSO* pso) {

    if (pso == NULL) {
      log_null_value(__FILE__, __LINE__);
    }

    switch (*pso->option)
    {
      case STEFCAL_LAMBDA:
        printf("STEFCAL_LAMBDA\n");
        break;
      case STEFCAL_IMPROVED:
        printf("STEFCAL_IMPROVED\n");
        break;
      case LEVENBERG_MARQUARDT_NON_ADAPTIVE_L1_L2:
        printf("LEVENBERG_MARQUARDT_NON_ADAPTIVE_L1_L2\n");
        break;
      case LEVENBERG_MARQUARDT_NON_ADAPTIVE_L1_L2_REDUCED_TIMESTEPS:
        printf("LEVENBERG_MARQUARDT_NON_ADAPTIVE_L1_L2_REDUCED_TIMESTEPS\n");
        break;  
      case LEVENBERG_MARQUARDT:
        printf("LEVENBERG_MARQUARDT\n");
        break;
      case LEVENBERG_MARQUARDT_REDUCED_TIMESTEPS:
        printf("LEVENBERG_MARQUARDT_REDUCED_TIMESTEPS\n");
        break;
      case ADAM:
        printf("ADAM\n");
        break;
      default:
        log_invalid_algorithm_type(__FILE__, __LINE__);
      break;
    }

    int seeds[] = {12345678, 23476934 ,98976433, 49052764};
    double snrs[] = {1000, 10, 0, -1};
    double max_amps[] = {2,3,5,10};

    //Opening DataDictionary and getting size of DataDictionary
    struct DataDictionary* data_dictionary = read_from_file_dd(pso->dd_directory, pso->dd_name);
    size_t dd_count = *(size_t*)data_dictionary->amount_items;
    size_t count = 0;

    struct PSOProgress* pso_progress = new_pp(&(size_t){50}, &(size_t){64}, pso->iterations_max, pso->num_particles);
  
    //Iterate through all the snrs, max_amps and seeds 
    for (size_t j = 0; j < 4; j++) {
      for (size_t h = 0; h < 4; h++) {
        for (size_t l = 0; l < 4; l++) {
            if (count >= dd_count) {

              //Create array of particles
              struct Particle** particles = malloc(*pso->num_particles * sizeof(struct Particle*));
              //initialise particle positions and velocities
              for (size_t p = 0; p < *pso->num_particles; p++) {
                struct array* position;
                if (*pso->option == LEVENBERG_MARQUARDT_NON_ADAPTIVE_L1_L2 || *pso->option == STEFCAL_LAMBDA || *pso->option == STEFCAL_LM_LAMBDA) {
                  position = random_position_bound1_pso(pso);
                } else if (*pso->option == STEFCAL_IMPROVED) {
                  position = random_position_bound2_pso(pso);
                } else {
                  position = random_position_pso(pso);
                }
                struct Particle* particle = new_particle(pso, pso->num_dimensions, position, pso->init_velocities);
                particles[p] = particle;
                free_a(position);
              }

              int seed = seeds[j];
              double snr = snrs[h];
              double max_amp = max_amps[l];
              char filename [256];
              snprintf(filename, sizeof(filename), "%s/sim_seed_%u_order_%d_snr_%.0lf_maxamp_%.0lf.sconfig", pso->simulators_directory, seed, *pso->order, snr, max_amp);
              struct Simulator* simulator = read_from_file_s(filename);
              struct Calibrator* calibrator = new_c(simulator);
              set_parallel_c(calibrator, pso->num_workers);

              //Clear the Global Best variables of the pso
              reset_pso(pso);
              //start iterative process of calculating the global best
              size_t global_best_update_count = 0;

              //This loop will run until there is convergence or max iteration is achieved
              for (size_t iter = 0; iter < *pso->iterations_max && global_best_update_count < 10; iter++) {
                //check and update the w, c1 and c2
                for (size_t p = 0; p < *pso->num_particles; p++) {
                  check_update_hyperparameters_particle(particles[p], pso->k);
                }
                // printf("Hyperparameters checked\n");

                //loop through all particles, run them and and update Global and Personal if needed
                for (size_t p = 0; p < *pso->num_particles; p++) {
                  
                  //check the option of the algorithm
                  double* average_iterations = &(double){0};
                  double* std_iterations = &(double){0};
                  double* last_residual_norm = &(double){0};
                  switch (*pso->option)
                  {
                  case LEVENBERG_MARQUARDT:
                    solve_G_LM(
                      calibrator, 
                      &(size_t){IMAX}, 
                      &(double){TAU}, 
                      &(double){*(double*)get_element_a(particles[p]->positions, (size_t[]){0})}, 
                      &(double){*(double*)get_element_a(particles[p]->positions, (size_t[]){1})}
                    );
                    
                    average_iterations = get_average_iterations_dc(calibrator->datacollector);
                    std_iterations = get_standard_deviation_iterations_dc(calibrator->datacollector);
                    last_residual_norm = get_last_residuals_norm_dc(calibrator->datacollector);

                    *particles[p]->current_iterations_mean = *average_iterations;
                    *particles[p]->current_iterations_std = *std_iterations; 
                    *particles[p]->current_residual_norm = *last_residual_norm;

                    free(last_residual_norm);
                    free(std_iterations);
                    free(average_iterations);
                    break;
                  case LEVENBERG_MARQUARDT_REDUCED_TIMESTEPS:
                    solve_G_LM_REDUCED_TIMESTEPS(
                      calibrator, 
                      &(size_t){IMAX}, 
                      &(double){TAU}, 
                      &(double){*(double*)get_element_a(particles[p]->positions, (size_t[]){0})}, 
                      &(double){*(double*)get_element_a(particles[p]->positions, (size_t[]){1})}
                    );
                    //set current values
                    
                    average_iterations = get_average_iterations_dc(calibrator->datacollector);
                    std_iterations = get_standard_deviation_iterations_dc(calibrator->datacollector);
                    last_residual_norm = get_last_residuals_norm_dc(calibrator->datacollector);

                    *particles[p]->current_iterations_mean = *average_iterations;
                    *particles[p]->current_iterations_std = *std_iterations; 
                    *particles[p]->current_residual_norm = *last_residual_norm;

                    free(last_residual_norm);
                    free(std_iterations);
                    free(average_iterations);
                    break;
                  default:
                    log_invalid_algorithm_type(__FILE__, __LINE__);
                    break;
                  }

                  increment_particle_pp(pso_progress);
                }

                //update personal best
                for (size_t p = 0; p < *pso->num_particles; p++) {
                  check_update_fitness_particle(particles[p]);
                }

                int global_update = 0;

                //update global best
                for (size_t p = 0; p < *pso->num_particles; p++) {
                  if (*particles[p]->current_residual_norm != NAN && *particles[p]->current_residual_norm != INFINITY) {
                    if (*particles[p]->pb_iterations_mean < *pso->global_best_iteration_mean) {
                      *pso->global_best_iteration_mean = *particles[p]->pb_iterations_mean;
                      *pso->global_best_iteration_std = *particles[p]->pb_iterations_std;
                      *pso->global_best_residual_norm = *particles[p]->pb_residual_norm;
                      copy_over_a(pso->global_best_positions,particles[p]->pb_positions, (int[]){-1,-1,-1}, (int[]){-1,-1,-1});
                      *pso->global_best_w = *particles[p]->w;
                      *pso->global_best_c1 = *particles[p]->c1;
                      *pso->global_best_c2 = *particles[p]->c2;
                      global_update = 1;
                    } else if (*particles[p]->pb_iterations_mean == *pso->global_best_iteration_mean) {
                      if (fabs(*particles[p]->pb_iterations_std - *pso->global_best_iteration_std) > EPSILON_PSO &&
                              *particles[p]->pb_iterations_std < *pso->global_best_iteration_std) {
                        *pso->global_best_iteration_mean = *particles[p]->pb_iterations_mean;
                        *pso->global_best_iteration_std = *particles[p]->pb_iterations_std;
                        *pso->global_best_residual_norm = *particles[p]->pb_residual_norm;
                        copy_over_a(pso->global_best_positions,particles[p]->pb_positions, (int[]){-1,-1,-1}, (int[]){-1,-1,-1});
                        *pso->global_best_w = *particles[p]->w;
                        *pso->global_best_c1 = *particles[p]->c1;
                        *pso->global_best_c2 = *particles[p]->c2;
                        global_update = 1;
                      } else if (fabs(*particles[p]->pb_iterations_std - *pso->global_best_iteration_std) <= EPSILON_PSO) {
                        if (fabs(*particles[p]->pb_residual_norm - *pso->global_best_residual_norm) > EPSILON_PSO &&
                                *particles[p]->pb_residual_norm < *pso->global_best_residual_norm) {
                          *pso->global_best_iteration_mean = *particles[p]->pb_iterations_mean;
                          *pso->global_best_iteration_std = *particles[p]->pb_iterations_std;
                          *pso->global_best_residual_norm = *particles[p]->pb_residual_norm;
                          copy_over_a(pso->global_best_positions,particles[p]->pb_positions, (int[]){-1,-1,-1}, (int[]){-1,-1,-1});
                          *pso->global_best_w = *particles[p]->w;
                          *pso->global_best_c1 = *particles[p]->c1;
                          *pso->global_best_c2 = *particles[p]->c2;
                          global_update = 1;
                        }
                      }
                    }
                  }
                }

                //if no update increment 
                if (global_update) {
                  global_best_update_count = 0;
                } else {
                  global_best_update_count += 1;
                }

                //update velocity
                for (size_t p = 0; p < *pso->num_particles; p++) {
                  update_velocity_particle(particles[p]);
                }
                
                //update position
                for (size_t p = 0; p < *pso->num_particles; p++) {
                  update_position_bounds_particle(particles[p]);
                }

                increment_iterations_pp(pso_progress);
              }

              //saving data to datacontainer
              char dc_name[256];
              sprintf(dc_name, "seed_%u_order_%d_snr_%.0lf_maxamp_%.0lf", seeds[j], *pso->order, snrs[h], max_amps[l]); 
              struct DataContainer* data_container = new_data_container_type_1_update_dd(&data_dictionary, dc_name, &(size_t){2});
              set_new_array_dcon(&data_container, &(size_t){0}, "positions", pso->global_best_positions);
              struct array* double_data = new_set_a(&(size_t){1},
                                                    (size_t[]){6}, 
                                                    &(enum DataType){DOUBLE}, 
                                                    (double[6]){
                                                        *pso->global_best_iteration_mean,
                                                        *pso->global_best_iteration_std,
                                                        *pso->global_best_residual_norm,
                                                        *pso->global_best_w,
                                                        *pso->global_best_c1,
                                                        *pso->global_best_c2,
                                                    }); 
              set_new_array_dcon(&data_container, &(size_t){1}, "double_data", double_data);  
              free_a(double_data);                                                                                         

              //save global variables to datacontainer
              free_dcon(data_container);
              free_c(calibrator);
              free_s(simulator);

              //free all particles
              for (size_t p = 0; p < *pso->num_particles; p++) {
                free_particle(particles[p]);
              }
              free(particles);
              free_dd(data_dictionary);
              increment_pso_pp(pso_progress);

              return 0;
            } else {
              increment_pso_no_effect_pp(pso_progress);
            }
            count += 1;
        }
      }
    }
    stop_free_pp(pso_progress, "");
    free_dd(data_dictionary);
    return 1;
}

/**
 * Generate random position bound1
 * This is for StEFCal
 * RETURNS:
 * - Random Position array
 * INPUTE:
 * - PSO struct pointer
*/
struct array* random_position_bound1_pso(struct PSO* pso) {
    if (*pso->num_dimensions != 2) {
      log_invalid_dimensions(__FILE__, __LINE__);
    }

    while (1) {
      struct array* position = random_position_pso(pso);

      //do checks to see if it is in x = -y + 0.5
      double* x = get_element_a(position, (size_t[]){0});
      double* y = get_element_a(position, (size_t[]){1});

      double x_bound = - *y + 0.5;
      double y_bound = - *x + 0.5;

      if (*x >= x_bound || *y >= y_bound) {
          return position;
      }
      free_a(position);
    }

    return NULL;
}

/**
 * Generate random position bound2
 * This is for StEFCal Improved
 * RETURNS:
 * - Random Position array
 * INPUTE:
 * - PSO struct pointer
*/
struct array* random_position_bound2_pso(struct PSO* pso) {
  if (*pso->num_dimensions != 2) {
    log_invalid_dimensions(__FILE__, __LINE__);
  }

  while (1) {
    struct array* position = random_position_pso(pso);

    //do checks to see if it is in x = -y + 0.5
    double* x = get_element_a(position, (size_t[]){0});
    double* y = get_element_a(position, (size_t[]){1});

    double x_bound = - *y + 0.5;
    double y_bound = - *x + 0.5;

    if ((*x >= x_bound || *y >= y_bound) && (*x <= -0.05 || *x >= 0.05) && (*y <= -0.05 || *y >= 0.05)) {
        return position;
    }
    free_a(position);
  }

  return NULL;
}

/**
 * Generate random position
 * RETURNS:
 * - Random Position array
 * INPUTE:
 * - PSO struct pointer
*/
struct array* random_position_pso(struct PSO* pso) {
    struct array* position = zeros_a(&(size_t){1}, (size_t[]){*pso->num_dimensions}, &(enum DataType){DOUBLE});
    for (size_t i = 0; i < *pso->num_dimensions; i++) {
      double random = generate_random_number_pso(pso, &(size_t){i});
      set_element_a(position, (size_t[]){i}, &random);
    }
    return position;
}

/**
 * Generate random value
 * RETURNS:
 * - Random value
 * INPUTE:
 * - PSO struct pointer
*/
double generate_random_number_pso(struct PSO* pso, size_t* dimension) {
    size_t index = *dimension * 2;
    double u = gsl_rng_uniform(pso->rng);
    double min = *(double*)get_element_a(pso->position_bounds, (size_t[]){index});
    double max = *(double*)get_element_a(pso->position_bounds, (size_t[]){index + 1});
    return min + u * (max-min);
}

/**
 * Resets Global best values
 * RETURNS:
 * - None
 * INPUTE:
 * - PSO struct pointer
*/
void reset_pso(struct PSO* pso) {
    *pso->global_best_residual_norm = INFINITY;
    *pso->global_best_iteration_mean = INFINITY;
    *pso->global_best_iteration_std = INFINITY;
    *pso->global_best_w = INFINITY;
    *pso->global_best_c1 = INFINITY;
    *pso->global_best_c2 = INFINITY;
}