/**
 * @file pso_progress.c
 * This is the implementation of the functions of pso_progress.h
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include "pso_progress.h"
 #include "stopwatch.h"
 #include <unistd.h>
 
 /**
  * This function creates a pso progress struct.
  * RETURNS:
  *  - PSOProgress struct pointer
  * INPUTS:
  *  - barWidth
  *  - pso_count_max
  *  - iteration_max
  *  - num_particles
  */
 struct PSOProgress* new_pp(size_t* barWidth, size_t* pso_count_max, size_t* iteration_max, size_t* num_particles) {
     struct PSOProgress* pso_progress = malloc(sizeof(struct PSOProgress));
 
     pso_progress->barWidth = malloc(sizeof(size_t));
     *pso_progress->barWidth = *barWidth;
     
     pso_progress->pso_count_max = malloc(sizeof(size_t));
     *pso_progress->pso_count_max = *pso_count_max;
     
     pso_progress->iteration_max = malloc(sizeof(size_t));
     *pso_progress->iteration_max = *iteration_max;
     
     pso_progress->num_particles = malloc(sizeof(size_t));
     *pso_progress->num_particles = *num_particles;
     
     pso_progress->stopwatch = new_sw();
     
     pso_progress->pso_count = malloc(sizeof(size_t));
     *pso_progress->pso_count = 0;
 
     pso_progress->pso_no_effect = malloc(sizeof(size_t));
     *pso_progress->pso_no_effect = 0;
     
     pso_progress->iteration = malloc(sizeof(size_t));
     *pso_progress->iteration = 0;
     
     pso_progress->particle = malloc(sizeof(size_t));
     *pso_progress->particle = 0;
     
     pso_progress->progress = malloc(sizeof(size_t));
     *pso_progress->progress = 0;
     
     pso_progress->totalSteps = malloc(sizeof(size_t));
     *pso_progress->totalSteps = (*pso_count_max) * (*iteration_max) * (*num_particles);
     
     pso_progress->progressRatio = malloc(sizeof(float));
     *pso_progress->progressRatio = 0.0f;
 
     return pso_progress;
 }
 
 /**
  * This frees a PSOProgress struct
  * RETURNS:
  *  - NONE
  * INPUTS:
  *  - PSOProgress struct pointer
  *  - message - stop message
  */
 void stop_free_pp(struct PSOProgress* pso_progress, char* message) {
     if (message != NULL) {
         printf("\n%s\n", message);
     }
     
     if (pso_progress != NULL) {
         if (pso_progress->barWidth != NULL) {
             free(pso_progress->barWidth);
         }
         if (pso_progress->pso_count_max != NULL) {
             free(pso_progress->pso_count_max);
         }
         if (pso_progress->iteration_max != NULL) {
             free(pso_progress->iteration_max);
         }
         if (pso_progress->num_particles != NULL) {
             free(pso_progress->num_particles);
         }
         if (pso_progress->pso_count != NULL) {
             free(pso_progress->pso_count);
         }
         if (pso_progress->pso_no_effect != NULL) {
             free(pso_progress->pso_no_effect);
         }
         if (pso_progress->iteration != NULL) {
             free(pso_progress->iteration);
         }
         if (pso_progress->particle != NULL) {
             free(pso_progress->particle);
         }
         if (pso_progress->progress != NULL) {
             free(pso_progress->progress);
         }
         if (pso_progress->totalSteps != NULL) {
             free(pso_progress->totalSteps);
         }
         if (pso_progress->progressRatio != NULL) {
             free(pso_progress->progressRatio);
         }
         if (pso_progress->stopwatch != NULL) {
             free_sw(pso_progress->stopwatch);
         }
         free(pso_progress);
     }
 }
 
 /**
  * This function will increment the pso of pso_progress
  * RETURNS:
  *  - NONE
  * INPUT:
  *  - PSOProgress struct pointer
  */
 void increment_pso_pp(struct PSOProgress* pso_progress) {
     if (pso_progress->pso_count != NULL && pso_progress->pso_count_max != NULL) {
         if (*pso_progress->pso_count < *pso_progress->pso_count_max) {
             *pso_progress->pso_count += 1;
             *pso_progress->iteration = 0;
             *pso_progress->particle = 0;
         }
     } else {
         // log_null_value(__FILE__, __LINE__); // Commented out - function not defined
         fprintf(stderr, "Error: NULL pointer in increment_pso_pp at %s:%d\n", __FILE__, __LINE__);
     }

     update_pp(pso_progress);
 }
 
 /**
  * This function will increment the pso no effect of pso_progress
  * RETURNS:
  * - NONE
  * INPUT:
  * - PSOProgress struct pointer
  */
 void increment_pso_no_effect_pp(struct PSOProgress* pso_progress) {
     if (pso_progress->pso_count != NULL && pso_progress->pso_count_max != NULL && pso_progress->pso_no_effect != NULL) {
         if (*pso_progress->pso_count < *pso_progress->pso_count_max) {
             *pso_progress->pso_no_effect += 1;
             *pso_progress->pso_count += 1;
             *pso_progress->iteration = 0;
             *pso_progress->particle = 0;
         }
     } else {
         // log_null_value(__FILE__, __LINE__); // Commented out - function not defined
         fprintf(stderr, "Error: NULL pointer in increment_pso_no_effect_pp at %s:%d\n", __FILE__, __LINE__);
     }

     update_pp(pso_progress);
 }
 
 /**
  * This function will increment the iterations of pso_progress
  * RETURNS:
  *  - NONE
  * INPUT:
  *  - PSOProgress struct pointer
  */
 void increment_iterations_pp(struct PSOProgress* pso_progress) {
     if (pso_progress->iteration_max != NULL && pso_progress->iteration != NULL) {
         if (*pso_progress->iteration < *pso_progress->iteration_max) {
             *pso_progress->iteration += 1;
             *pso_progress->particle = 0;
         }
     } else {
         // log_null_value(__FILE__, __LINE__); // Commented out - function not defined
         fprintf(stderr, "Error: NULL pointer in increment_iterations_pp at %s:%d\n", __FILE__, __LINE__);
     }

     update_pp(pso_progress);
 }
 
 /**
  * This function will increment the particle of pso_progress
  * RETURNS:
  *  - NONE
  * INPUT:
  *  - PSOProgress struct pointer
  */
 void increment_particle_pp(struct PSOProgress* pso_progress) {
     if (pso_progress->num_particles != NULL && pso_progress->particle != NULL) {
         if (*pso_progress->particle < *pso_progress->num_particles) {
             *pso_progress->particle += 1;
         }
     } else {
         // log_null_value(__FILE__, __LINE__); // Commented out - function not defined
         fprintf(stderr, "Error: NULL pointer in increment_particle_pp at %s:%d\n", __FILE__, __LINE__);
     }

     update_pp(pso_progress);
 }
 
 /**
  * This function calculates current progress based on PSO, iteration, and particle counts
  * RETURNS:
  * - current progress value
  * INPUT:
  * - PSOProgress struct pointer
  */
 size_t calculate_progress_pp(struct PSOProgress* pso_progress) {
     if (pso_progress == NULL || 
         pso_progress->pso_count == NULL || 
         pso_progress->pso_no_effect == NULL ||
         pso_progress->iteration == NULL || 
         pso_progress->particle == NULL ||
         pso_progress->iteration_max == NULL ||
         pso_progress->num_particles == NULL) {
         return 0;
     }
     
     size_t current_progress = 0;
     
     // Add completed effective PSOs (full work done)
     if (*pso_progress->pso_count > 0) {
        current_progress += (*pso_progress->pso_count - 1) * (*pso_progress->iteration_max) * (*pso_progress->num_particles);
     }

     current_progress += (*pso_progress->iteration) * (*pso_progress->num_particles);
         
     // Add completed particles in current iteration
     current_progress += *pso_progress->particle;
     
     return current_progress;
 }
 
 /**
  * This function estimates time remaining
  * RETURNS:
  * - estimated seconds remaining
  * INPUT:
  * - PSOProgress struct pointer
  */
 double estimate_time_remaining_pp(struct PSOProgress* pso_progress) {
     if (pso_progress == NULL || pso_progress->stopwatch == NULL || 
         pso_progress->progress == NULL || pso_progress->totalSteps == NULL ||
         pso_progress->pso_count == NULL || pso_progress->pso_no_effect == NULL) {
         return -1.0; // Invalid estimate
     }
     
     double elapsed_time = elapsed_sw(pso_progress->stopwatch);
     
     if (*pso_progress->progress == 0) {
         return -1.0; // Cannot estimate with no progress
     }

     size_t effective_psos_completed = (*pso_progress->pso_count) - (*pso_progress->pso_no_effect);
     size_t effective_work_done = 0;
     if (effective_psos_completed > 0) {
        effective_work_done = (effective_psos_completed-1) * (*pso_progress->iteration_max) * (*pso_progress->num_particles);
     }

     effective_work_done += (*pso_progress->iteration) * (*pso_progress->num_particles);
     effective_work_done += *pso_progress->particle;
     
     if (effective_work_done == 0) {
         return -1.0; // No effective work done yet
     }
     
     // Calculate remaining effective work
     size_t remaining_psos = (*pso_progress->pso_count_max) - (*pso_progress->pso_count);

     size_t remaining_effective_work = 0;

     if (remaining_psos > 0) {
        remaining_effective_work += (remaining_psos - 1) * (*pso_progress->iteration_max) * (*pso_progress->num_particles);
     }

     remaining_effective_work += (*pso_progress->iteration_max - *pso_progress->iteration) * (*pso_progress->num_particles);
     remaining_effective_work += (*pso_progress->num_particles) - (*pso_progress->particle);
     
     double time_per_effective_step = elapsed_time / effective_work_done;
     return time_per_effective_step * remaining_effective_work;
 }
 
 /**
  * This function will update the progress bar and show ETA
  * RETURNS:
  *  - NONE
  * INPUT:
  *  - PSOProgress struct pointer
  */
 void update_pp(struct PSOProgress* pso_progress) {
     if (pso_progress == NULL) {
         return;
     }
     
     // Calculate current progress
     size_t current_progress = calculate_progress_pp(pso_progress);
     
     // Only update if progress has actually changed
     if (current_progress != *pso_progress->progress) {
         *pso_progress->progress = current_progress;
         *pso_progress->progressRatio = (float)(*pso_progress->progress) / (*pso_progress->totalSteps);
         
         int barLength = (int)(*pso_progress->progressRatio * (*pso_progress->barWidth));
         
         // Clear the line and print progress bar
         printf("\r");
         printf("[");
         for (int i = 0; i < *pso_progress->barWidth; ++i) {
             if (i < barLength) {
                 printf("=");
             } else {
                 printf(" ");
             }
         }
         
         // Calculate and display ETA
         double eta = estimate_time_remaining_pp(pso_progress);
         double elapsed = elapsed_sw(pso_progress->stopwatch);
         
         printf("] %d%% ", (int)(*pso_progress->progressRatio * 100));
         printf("PSO:%zu/%zu(skip:%zu) Iter:%zu/%zu Part:%zu/%zu ",
            *pso_progress->pso_count, *pso_progress->pso_count_max, *pso_progress->pso_no_effect,
            *pso_progress->iteration, *pso_progress->iteration_max,
            *pso_progress->particle, *pso_progress->num_particles);
         
         if (eta > 0) {
             int eta_hours = (int)(eta / 3600);
             int eta_minutes = (int)((eta - eta_hours * 3600) / 60);
             int eta_seconds = (int)(eta - eta_hours * 3600 - eta_minutes * 60);
             
             if (eta_hours > 0) {
                 printf("ETA: %02d:%02d:%02d", eta_hours, eta_minutes, eta_seconds);
             } else {
                 printf("ETA: %02d:%02d", eta_minutes, eta_seconds);
             }
         }
         
         printf(" Elapsed: %.1fs", elapsed);
         
         fflush(stdout);
     }
 }